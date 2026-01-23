#include <Arduino.h>
#include <WiFi.h>
#include <esp_task_wdt.h>

#include "config.h"
#include "wifi_mgr.h"
#include "e131_mgr.h"
#include "dmx_map.h"
#include "leds.h"

#include "web_portal.h"


// WDT and heartbeat
constexpr int WDT_TIMEOUT_S = 5;
constexpr uint32_t HEARTBEAT_MS = 5000;

// stats
static uint8_t prevSeq = 0;
static uint32_t pktCount = 0;
static uint32_t lossCount = 0;
static uint32_t hbStartMs = 0;

// idle timing
static uint32_t lastActiveMs = 0;
static bool wasWifiUp = false;

// buffers
static uint8_t pixBuf[MAX_PIXELS * 3];
static uint8_t dmxBuf[512];

// keepalive pulse window
static uint32_t keepaliveUntilMs = 0;
static inline bool inKeepaliveWindow()
{
  return millis() < keepaliveUntilMs;
}

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
  webPortalBegin(); // start the web UI (http://<esp-ip>/)
  e131Init();
  ledsWriteAll(0, 0, 0, 0, 0);

  esp_task_wdt_init(WDT_TIMEOUT_S, true);
  esp_task_wdt_add(NULL);

  hbStartMs = millis();
  lastActiveMs = hbStartMs;
}

void loop()
{
  esp_task_wdt_reset();
  wifiEnsure();

  if (WiFi.status() == WL_CONNECTED && !wasWifiUp)
  {
    e131Init();
    Serial.println("E131 reinit after WiFi reconnect");
  }
  wasWifiUp = (WiFi.status() == WL_CONNECTED);

  uint16_t dlen = 0;
  uint8_t seq = 0;

  while (e131PollRaw(dmxBuf, dlen, seq))
  {
    // stats
    pktCount++;
    if (pktCount > 1)
    {
      uint8_t expect = uint8_t(prevSeq + 1);
      if (seq != expect)
        lossCount++;
    }
    prevSeq = seq;

    if (START_ADDR < 1 || START_ADDR > 512)
      continue;

    const uint16_t base = START_ADDR - 1; // 0-based
    uint8_t gi = dmxBuf[base];

    bool litThisFrame = false;

    if (RUN_MODE == MODE_4CH)
    {
      // I,R,G,B at START_ADDR..+3
      uint8_t r = 0, g = 0, b = 0;
      if (base + 3 < 512)
      {
        r = dmxBuf[base + 1];
        g = dmxBuf[base + 2];
        b = dmxBuf[base + 3];
      }
      if (gi == 0 && MIN_IDLE_MASTER)
        gi = MIN_IDLE_MASTER;

      if (!(inKeepaliveWindow() && gi == 0 && r == 0 && g == 0 && b == 0))
      {
        ledsWriteAll(r, g, b, 0, gi);
      }
      litThisFrame = (gi && (r | g | b)) || MIN_IDLE_MASTER;
    }
    else
    { // MODE_PERPIXEL
      // map per-pixel block starting at START+1
      bool useBroadcast = false;
      uint8_t br = 0, bg = 0, bb = 0;
      DmxFrameView fv{dmxBuf, 512, uint16_t(base)};
      if (dmxMap_PerPixelRGB_GlobalI(fv, pixBuf, NUM_PIXELS, gi, useBroadcast, br, bg, bb))
      {
        auto anyNonZero = [](const uint8_t *p, int n)
        {
          for (int i = 0; i < n; i++)
            if (p[i])
              return true;
          return false;
        };
        bool hasContent = false;
        if (useBroadcast)
          hasContent = (gi > 0) && (br || bg || bb);
        else
          hasContent = (gi > 0) && anyNonZero(pixBuf, NUM_PIXELS * 3);

        if (!hasContent && MIN_IDLE_MASTER)
        {
          gi = MIN_IDLE_MASTER;
          hasContent = true;
        }

        if (!(inKeepaliveWindow() && !hasContent))
        {
          if (useBroadcast)
            ledsWriteAll(br, bg, bb, 0, gi);
          else
            ledsWriteStripRGB(pixBuf, NUM_PIXELS, gi);
        }
        litThisFrame = hasContent;
      }
    }

    if (litThisFrame)
      lastActiveMs = millis();
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

  // keepalive pulse (optional)
  if (KEEPALIVE_ON_TIMEOUT && (millis() - lastActiveMs > IDLE_TIMEOUT_MS))
  {
    keepaliveUntilMs = millis() + KEEPALIVE_PULSE_MS;
    ledsWriteSingle(KEEPALIVE_PIXEL, 0, KEEPALIVE_LEVEL, 0, 0, KEEPALIVE_LEVEL);
  }

  // force blackout when idle and no pulse or floor
  if (!KEEPALIVE_ON_TIMEOUT && !MIN_IDLE_MASTER && (millis() - lastActiveMs > IDLE_TIMEOUT_MS))
  {
    ledsWriteAll(0, 0, 0, 0, 0);
  }
}
