#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "wifi_mgr.h"
#include "leds.h"
#include "e131_mgr.h"
#include <esp_task_wdt.h>

constexpr int WDT_TIMEOUT_S = 5;

// printing
constexpr bool PRINT_ON_CHANGE = true;
constexpr uint8_t CHANGE_THRESH = 1;
constexpr uint8_t DEAD_BAND = 1;
constexpr uint32_t HEARTBEAT_MS = 5000;

// state
static uint8_t lastR = 255, lastG = 255, lastB = 255, lastW = 255, lastDim = 255;
static uint8_t prevSeq = 0;
static uint32_t pktCount = 0;
static uint32_t lossCount = 0;
static uint32_t hbStartMs = 0;
static uint32_t lastPacketMs = 0;
static bool wasWifiUp = false;

void setup()
{
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println("=== Boot ===");

  ledsInit();
  ledsSetBrightnessCap(DEFAULT_BRIGHTNESS_CAP);

  wifiInit();
  ledsShowIPOnce(wifiIP());

  e131Init();
  ledsWriteAll(0, 0, 0, 0, 0);

  esp_task_wdt_init(WDT_TIMEOUT_S, true);
  esp_task_wdt_add(NULL);

  hbStartMs = millis();
}

void loop()
{
  esp_task_wdt_reset();
  wifiEnsure();

  bool wifiUp = (WiFi.status() == WL_CONNECTED);
  if (wifiUp && !wasWifiUp)
  {
    e131Init();
    Serial.println("E131 reinit after WiFi reconnect");
  }
  wasWifiUp = wifiUp;

  uint8_t r = 0, g = 0, b = 0, w = 0, dim = 0, seq = 0;
  bool anyPacket = false;
  bool changed = false;

  while (e131Poll(r, g, b, w, dim, seq))
  {
    auto diff = [](uint8_t a, uint8_t b)
    { return (a > b) ? (a - b) : (b - a); };
    changed = changed ||
              diff(r, lastR) >= DEAD_BAND ||
              diff(g, lastG) >= DEAD_BAND ||
              diff(b, lastB) >= DEAD_BAND ||
              diff(w, lastW) >= DEAD_BAND ||
              diff(dim, lastDim) >= DEAD_BAND;

    if (changed)
      ledsWriteAll(r, g, b, w, dim);

    lastPacketMs = millis();
    anyPacket = true;
    pktCount++;

    if (pktCount > 1)
    {
      uint8_t expect = uint8_t(prevSeq + 1);
      if (seq != expect)
        lossCount++;
    }
    prevSeq = seq;

    lastR = r;
    lastG = g;
    lastB = b;
    lastW = w;
    lastDim = dim;
  }

  // print on change
  static uint32_t lastChangePrint = 0;
  if (anyPacket && changed && millis() - lastChangePrint > 100)
  {
    lastChangePrint = millis();
    Serial.print("DMX i,r,g,b,w = ");
    Serial.print((int)lastDim);
    Serial.print(",");
    Serial.print((int)lastR);
    Serial.print(",");
    Serial.print((int)lastG);
    Serial.print(",");
    Serial.print((int)lastB);
    Serial.print(",");
    Serial.println((int)lastW);
  }

  // heartbeat
  if (millis() - hbStartMs >= HEARTBEAT_MS)
  {
    uint32_t elapsed = millis() - hbStartMs;
    float pps = elapsed ? (1000.0f * pktCount / elapsed) : 0.0f;
    Serial.print("U");
    Serial.print(E131_UNIVERSE);
    Serial.print(" pps ");
    Serial.print(pps, 1);
    Serial.print(" loss ");
    Serial.print(lossCount);
    Serial.print(" RSSI ");
    Serial.print(WiFi.RSSI());
    Serial.print(" dBm heap ");
    Serial.println(ESP.getFreeHeap());
    pktCount = 0;
    lossCount = 0;
    hbStartMs = millis();
  }

  // timeout behavior
  if (millis() - lastPacketMs > DMX_TIMEOUT_MS)
  {
    if (KEEPALIVE_ON_TIMEOUT)
    {
      ledsWriteSingle(KEEPALIVE_PIXEL, 0, 0, 0, KEEPALIVE_LEVEL, KEEPALIVE_LEVEL);
    }
    else
    {
      ledsWriteAll(0, 0, 0, 0, 0);
    }
  }
}
