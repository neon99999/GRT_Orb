#include "leds.h"
#include "config.h"

static Adafruit_NeoPixel strip(NUM_PIXELS, LED_PIN, PIXEL_ORDER);
static uint8_t s_cap = DEFAULT_BRIGHTNESS_CAP;

static const uint8_t PROGMEM gamma8Table[256] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,3,
  3,3,3,4,4,4,5,5,6,6,7,7,8,8,9,10,10,11,12,13,13,14,15,16,17,18,19,20,21,22,24,25,
  26,27,29,30,32,33,35,36,38,40,41,43,45,47,49,51,53,55,57,59,61,63,66,68,70,73,75,78,80,83,86,88,
  91,94,97,100,103,106,109,112,115,118,122,125,128,132,135,139,142,146,150,153,157,161,165,169,173,177,181,185,190,194,198,203,
  207,212,216,221,226,230,235,240,245,250,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255
};
static inline uint8_t g8(uint8_t x){ return pgm_read_byte(&gamma8Table[x]); }
static inline uint32_t pack(uint8_t r,uint8_t g,uint8_t b,uint8_t w){ return strip.Color(r,g,b,w); }

// cache last color to avoid redundant .show()
static uint32_t lastPacked = 0;
static bool lastValid = false;

void ledsInit(){
  strip.begin();
  strip.setBrightness(255);
  strip.show();
}

void ledsSetBrightnessCap(uint8_t cap){ s_cap = cap; }

void ledsWriteAll(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint8_t master){
  r = g8(min<uint8_t>(r, s_cap));
  g = g8(min<uint8_t>(g, s_cap));
  b = g8(min<uint8_t>(b, s_cap));
  w = g8(min<uint8_t>(w, s_cap));
  r = (uint16_t)r * master / 255;
  g = (uint16_t)g * master / 255;
  b = (uint16_t)b * master / 255;
  w = (uint16_t)w * master / 255;

  uint32_t c = pack(r,g,b,w);
  if (lastValid && c == lastPacked) return;
  lastPacked = c;
  lastValid = true;

  for (int i = 0; i < NUM_PIXELS; i++) strip.setPixelColor(i, c);
  strip.show();
}

// show 1 octet as bits on pixels 0..7
static void showOctet(uint8_t val, uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint16_t ms){
  strip.clear();
  for (int bit = 7; bit >= 0; --bit){
    int idx = 7 - bit;
    if (val & (1 << bit)) strip.setPixelColor(idx, pack(r,g,b,w));
  }
  strip.show();
  delay(ms);
}

void ledsShowIPOnce(IPAddress ip){
  showOctet(ip[0], 60, 0, 0, 0, 900);
  showOctet(ip[1], 0, 60, 0, 0, 900);
  showOctet(ip[2], 0, 0, 60, 0, 900);
  showOctet(ip[3], 0, 0, 0, 40, 1200);
  strip.clear();
  strip.show();
}
