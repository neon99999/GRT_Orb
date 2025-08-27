#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "wifi_mgr.h"
#include "leds.h"
#include "e131_mgr.h"

// print controls
constexpr bool     PRINT_ON_CHANGE = true;
constexpr uint8_t  CHANGE_THRESH   = 1;        // ignore tiny changes
constexpr uint32_t HEARTBEAT_MS    = 5000;     // 5 s stats

// state
static uint8_t  lastR = 255, lastG = 255, lastB = 255, lastW = 255, lastDim = 255;
static uint8_t  prevSeq = 0;
static uint32_t pktCount = 0;
static uint32_t lossCount = 0;
static uint32_t hbStartMs = 0;
static uint32_t lastPacketMs = 0;

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

  hbStartMs = millis();
}

void loop(){
  wifiEnsure();

  uint8_t r=0,g=0,b=0,w=0,dim=0,seq=0;
  bool anyPacket = false;
  bool changed   = false;

  while (e131Poll(r,g,b,w,dim,seq)){
    // only write LEDs when something changes
    auto diff = [](uint8_t a, uint8_t b){ return (a>b)?(a-b):(b-a); };
    changed = changed ||
              diff(r,lastR) >= CHANGE_THRESH ||
              diff(g,lastG) >= CHANGE_THRESH ||
              diff(b,lastB) >= CHANGE_THRESH ||
              diff(w,lastW) >= CHANGE_THRESH ||
              diff(dim,lastDim) >= CHANGE_THRESH;

    if (changed) ledsWriteAll(r,g,b,w,dim);

    lastPacketMs = millis();
    anyPacket = true;
    pktCount++;

    if (pktCount > 1){
      uint8_t expect = uint8_t(prevSeq + 1);
      if (seq != expect) lossCount++;
    }
    prevSeq = seq;

    lastR=r; lastG=g; lastB=b; lastW=w; lastDim=dim;
  }

  // print on change, rate limited
  static uint32_t lastChangePrint = 0;
  if (anyPacket && changed && millis() - lastChangePrint > 100){
    lastChangePrint = millis();
    Serial.print("DMX i,r,g,b,w = ");
    Serial.print((int)lastDim); Serial.print(",");
    Serial.print((int)lastR);   Serial.print(",");
    Serial.print((int)lastG);   Serial.print(",");
    Serial.print((int)lastB);   Serial.print(",");
    Serial.println((int)lastW);

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
    pktCount = 0;
    lossCount = 0;
    hbStartMs = millis();
  }

  // blackout on timeout
  if (millis() - lastPacketMs > DMX_TIMEOUT_MS) ledsWriteAll(0,0,0,0,0);
}
