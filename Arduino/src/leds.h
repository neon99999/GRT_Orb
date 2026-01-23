// leds.h
#pragma once
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>

void ledsInit();
void ledsSetBrightnessCap(uint8_t cap);

void ledsWriteAll(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint8_t master);
void ledsWriteStripRGB(const uint8_t *rgb, uint16_t count, uint8_t master);
void ledsWriteSingle(uint16_t idx, uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint8_t master);

void ledsShowIPOnce(IPAddress ip);
