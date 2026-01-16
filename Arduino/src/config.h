#pragma once
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>

// ---------- Compile-time hardware ----------
constexpr int LED_PIN    = 5;
constexpr int STATUS_LED = 2;
#define PIXEL_ORDER (NEO_GRB + NEO_KHZ800)

// NeoPixel needs a compile-time length at construction if you use the
// convenience ctor. We'll construct dynamically, but also keep a safe max
#ifndef MAX_PIXELS
#define MAX_PIXELS 170   // one sACN universe of RGB plus master + broadcast
#endif

// ---------- Runtime-tunable (live in config.cpp) ----------
extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;

extern uint16_t E131_UNIVERSE;
extern uint16_t START_ADDR;          // 321 for master + per-pixel RGB
extern bool     USE_UNICAST;

extern int      NUM_PIXELS;          // strip length at runtime

extern uint8_t  DEFAULT_BRIGHTNESS_CAP;
extern uint32_t DMX_TIMEOUT_MS;

// Static IP tuple
extern bool      USE_STATIC_IP;
extern IPAddress STATIC_IP;
extern IPAddress STATIC_GW;
extern IPAddress STATIC_SN;
extern IPAddress STATIC_DNS;

// Keep-alive behavior
extern bool     KEEPALIVE_ON_TIMEOUT;
extern uint8_t  KEEPALIVE_LEVEL;
extern uint8_t  KEEPALIVE_PIXEL;
extern uint32_t KEEPALIVE_PULSE_MS;
extern uint32_t KEEPALIVE_PERIOD_MS;

extern uint16_t IDLE_TIMEOUT_MS;   // how long of blackout before keepalive
extern uint8_t  MIN_IDLE_MASTER;   // optional floor for master during blackout
