#include <Arduino.h>
#include "config.h"
#include "wifi_mgr.h"
#include "leds.h"
#include "e131_mgr.h"

static uint32_t lastPacketMs = 0;
static uint8_t  lastSeq = 0;

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
}

void loop(){
  wifiEnsure();

  uint8_t r,g,b,w,dim,seq;
  bool got = false;
  while (e131Poll(r,g,b,w,dim,seq)){
    ledsWriteAll(r,g,b,w,dim);
    lastSeq = seq;
    lastPacketMs = millis();
    got = true;
  }

  static uint32_t lastPrint = 0;
  if (got && millis() - lastPrint > 500){
    lastPrint = millis();
    Serial.print("U"); Serial.print(E131_UNIVERSE);
    Serial.print(" seq "); Serial.print(lastSeq);
    Serial.print(" DMX r,g,b,w,dim = ");
    Serial.print((int)r); Serial.print(",");
    Serial.print((int)g); Serial.print(",");
    Serial.print((int)b); Serial.print(",");
    Serial.print((int)w); Serial.print(",");
    Serial.println((int)dim);
  }

  if (millis() - lastPacketMs > DMX_TIMEOUT_MS){
    ledsWriteAll(0,0,0,0,0);
  }
}
