#include "leds.h"
#include "config.h"

static Adafruit_NeoPixel strip;     // runtime-sized in ledsInit()
static uint8_t s_cap = 255;         // overridden at init

static inline uint8_t g8(uint8_t x) {
  return Adafruit_NeoPixel::gamma8(x);
}

void ledsInit(){
  strip.updateType(PIXEL_ORDER);
  strip.setPin(LED_PIN);
  strip.updateLength(NUM_PIXELS);
  strip.begin();
  strip.setBrightness(255);
  strip.show();
  s_cap = DEFAULT_BRIGHTNESS_CAP;
}

void ledsSetBrightnessCap(uint8_t cap){ s_cap = cap; }

void ledsWriteAll(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint8_t master){
  // gamma
  r = g8(r); g = g8(g); b = g8(b); w = g8(w);
  // master
  r = (uint16_t)r * master / 255;
  g = (uint16_t)g * master / 255;
  b = (uint16_t)b * master / 255;
  w = (uint16_t)w * master / 255;
  // cap last
  if (r > s_cap) r = s_cap;
  if (g > s_cap) g = s_cap;
  if (b > s_cap) b = s_cap;
  if (w > s_cap) w = s_cap;

  for (int i = 0; i < NUM_PIXELS; i++) strip.setPixelColor(i, strip.Color(r,g,b,w));
  strip.show();
}

void ledsWriteStripRGB(const uint8_t* rgb, uint16_t count, uint8_t master){
  if (!rgb) return;
  if (count > (uint16_t)NUM_PIXELS) count = (uint16_t)NUM_PIXELS;

  for (uint16_t i = 0; i < count; i++){
    uint8_t r = rgb[3*i + 0];
    uint8_t g = rgb[3*i + 1];
    uint8_t b = rgb[3*i + 2];
    // gamma
    r = g8(r); g = g8(g); b = g8(b);
    // master
    r = (uint16_t)r * master / 255;
    g = (uint16_t)g * master / 255;
    b = (uint16_t)b * master / 255;
    // cap last
    if (r > s_cap) r = s_cap;
    if (g > s_cap) g = s_cap;
    if (b > s_cap) b = s_cap;

    strip.setPixelColor(i, strip.Color(r,g,b));
  }
  // clear tail if count < NUM_PIXELS
  for (uint16_t i = count; i < (uint16_t)NUM_PIXELS; i++) strip.setPixelColor(i, 0);
  strip.show();
}

void ledsWriteSingle(uint16_t idx, uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint8_t master){
  // gamma
  r = g8(r); g = g8(g); b = g8(b); w = g8(w);
  // master
  r = (uint16_t)r * master / 255;
  g = (uint16_t)g * master / 255;
  b = (uint16_t)b * master / 255;
  w = (uint16_t)w * master / 255;
  // cap last
  if (r > s_cap) r = s_cap;
  if (g > s_cap) g = s_cap;
  if (b > s_cap) b = s_cap;
  if (w > s_cap) w = s_cap;

  for (int i = 0; i < NUM_PIXELS; i++)
    strip.setPixelColor(i, i == (int)idx ? strip.Color(r,g,b,w) : 0);
  strip.show();
}

void ledsShowIPOnce(IPAddress ip){
  static bool shown = false;
  if (shown) return;
  shown = true;

  const uint8_t master = 64;
  for (int i = 0; i < NUM_PIXELS; ++i){
    uint8_t r = 0, g = 32, b = 0, w = 0;
    r = g8(r); g = g8(g); b = g8(b); w = g8(w);
    r = (uint16_t)r * master / 255;
    g = (uint16_t)g * master / 255;
    b = (uint16_t)b * master / 255;
    w = (uint16_t)w * master / 255;

    for (int j = 0; j < NUM_PIXELS; ++j)
      strip.setPixelColor(j, j == i ? strip.Color(r,g,b,w) : 0);
    strip.show();
    delay(15);
  }
  for (int j = 0; j < NUM_PIXELS; ++j) strip.setPixelColor(j, 0);
  strip.show();

  Serial.print("IP: ");
  Serial.println(ip);
}
