#include <Arduino.h>
#include <WiFi.h>
#include <esp_task_wdt.h>

#include "config.h"
#include "wifi_mgr.h"
#include "e131_mgr.h"
#include "dmx_map.h"
#include "leds.h"

constexpr int      WDT_TIMEOUT_S  = 5;
constexpr uint32_t HEARTBEAT_MS   = 5000;

// stats
static uint8_t  prevSeq      = 0;
static uint32_t pktCount     = 0;
static uint32_t lossCount    = 0;
static uint32_t hbStartMs    = 0;

// activity tracking
static uint32_t lastActiveMs     = 0;   // last time we lit any LED (or applied idle floor)
static bool     wasWifiUp        = false;

// keepalive pulse window end-time
static uint32_t keepaliveUntilMs = 0;

// buffers
static uint8_t pixBuf[MAX_PIXELS * 3];
static uint8_t dmxBuf[512];

static inline bool inKeepaliveWindow(){
  return (int32_t)(keepaliveUntilMs - millis()) > 0;
}

void setup(){
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println("=== Boot ===");

  ledsInit();
  ledsSetBrightnessCap(DEFAULT_BRIGHTNESS_CAP);

  wifiInit();
  ledsShowIPOnce(wifiIP());

  e131Init();
  ledsWriteAll(0,0,0,0,0);

  esp_task_wdt_init(WDT_TIMEOUT_S, true);
  esp_task_wdt_add(NULL);

  hbStartMs    = millis();
  lastActiveMs = hbStartMs;
}

void loop(){
  esp_task_wdt_reset();
  wifiEnsure();

  // reinit E1.31 after WiFi reconnect
  bool wifiUp = (WiFi.status() == WL_CONNECTED);
  if (wifiUp && !wasWifiUp){
    e131Init();
    Serial.println("E131 reinit after WiFi reconnect");
  }
  wasWifiUp = wifiUp;

  // process sACN
  uint16_t dlen = 0;
  uint8_t  seq  = 0;

  while (e131PollRaw(dmxBuf, dlen, seq)){
    pktCount++;
    if (pktCount > 1){
      uint8_t expect = uint8_t(prevSeq + 1);
      if (seq != expect) lossCount++;
    }
    prevSeq = seq;

    DmxFrameView fv{ dmxBuf, 512, uint16_t(START_ADDR ? START_ADDR - 1 : 0) };

    uint8_t gi = 255, br=0, bg=0, bb=0;
    bool useBroadcast = false;

    if (dmxMap_PerPixelRGB_GlobalI(fv, pixBuf, NUM_PIXELS, gi, useBroadcast, br, bg, bb)){
      // detect actual light content after master
      auto anyNonZero = [](const uint8_t* p, int n){
        for (int i=0;i<n;i++) if (p[i]) return true;
        return false;
      };
      bool hasContent = false;
      if (useBroadcast){
        hasContent = (gi > 0) && (br || bg || bb);
      } else {
        hasContent = (gi > 0) && anyNonZero(pixBuf, NUM_PIXELS*3);
      }

      // steady floor during blackout (optional)
      if (!hasContent && MIN_IDLE_MASTER){
        gi = MIN_IDLE_MASTER;
        hasContent = true; // we will light something
      }

      // if a keepalive pulse is active and this frame is black, ignore it
      if (!hasContent && inKeepaliveWindow()){
        continue; // do not overwrite the pulse with zeros
      }

      // drive LEDs
      if (useBroadcast){
        ledsWriteAll(br, bg, bb, 0, gi);
      } else {
        ledsWriteStripRGB(pixBuf, NUM_PIXELS, gi);
      }

      if (hasContent){
        lastActiveMs = millis();
      }
    }
  }

  // heartbeat
  if (millis() - hbStartMs >= HEARTBEAT_MS){
    uint32_t elapsed = millis() - hbStartMs;
    float pps = elapsed ? (1000.0f * pktCount / elapsed) : 0.0f;
    Serial.print("U"); Serial.print(E131_UNIVERSE);
    Serial.print(" pps "); Serial.print(pps, 1);
    Serial.print(" loss "); Serial.print(lossCount);
    Serial.print(" RSSI "); Serial.print(WiFi.RSSI());
    Serial.print(" dBm heap "); Serial.println(ESP.getFreeHeap());
    pktCount  = 0;
    lossCount = 0;
    hbStartMs = millis();
  }

  // keepalive when visually idle long enough (works even if DMX streams zeros)
  if (KEEPALIVE_ON_TIMEOUT && (millis() - lastActiveMs > IDLE_TIMEOUT_MS)){
    // start or extend pulse window
    keepaliveUntilMs = millis() + KEEPALIVE_PULSE_MS;
    // WS2812B is RGB: faint green tick on one pixel
    ledsWriteSingle(KEEPALIVE_PIXEL, 0, KEEPALIVE_LEVEL, 0, 0, KEEPALIVE_LEVEL);
  }

  // if not using pulse and no floor requested, ensure blackout
  if (!KEEPALIVE_ON_TIMEOUT && !MIN_IDLE_MASTER && (millis() - lastActiveMs > IDLE_TIMEOUT_MS)){
    ledsWriteAll(0,0,0,0,0);
  }
}
