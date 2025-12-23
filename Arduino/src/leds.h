#pragma once
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

void ledsInit();
void ledsSetBrightnessCap(uint8_t cap);
void ledsWriteAll(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint8_t master);
void ledsShowIPOnce(IPAddress ip);
void ledsWriteSingle(uint16_t idx, uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint8_t master);
