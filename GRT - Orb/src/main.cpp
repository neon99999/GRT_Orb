#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN    5
#define NUM_PIXELS 12

// Try GRBW first. If colors look swapped, change to NEO_RGBW.
Adafruit_NeoPixel strip(NUM_PIXELS, LED_PIN, NEO_GRBW + NEO_KHZ800);

void showOne(int i, uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint16_t ms) {
  strip.clear();
  strip.setPixelColor(i, strip.Color(r, g, b, w));
  strip.show();
  delay(ms);
}

void setup() {
  strip.begin();
  strip.setBrightness(30);   // low brightness
  strip.show();              // all off
}

void loop() {
  // Walk one pixel around the ring testing R, G, B, then W
  for (int i = 0; i < NUM_PIXELS; i++) showOne(i, 60, 0, 0, 0, 120); // red
  for (int i = 0; i < NUM_PIXELS; i++) showOne(i, 0, 60, 0, 0, 120); // green
  for (int i = 0; i < NUM_PIXELS; i++) showOne(i, 0, 0, 60, 0, 120); // blue
  for (int i = 0; i < NUM_PIXELS; i++) showOne(i, 0, 0, 0, 40, 120); // white channel
}
