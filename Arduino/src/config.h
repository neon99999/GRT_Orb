// config.h
#pragma once
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>

// ---------- Compile-time hardware ----------
constexpr int LED_PIN = 5;
constexpr int STATUS_LED = 2;
#define PIXEL_ORDER (NEO_GRB + NEO_KHZ800)

// ---------- Reserved "always-on" pixel ----------
// Physical pixel 0 is not addressable from DMX. It is always on at a low level.
// The first DMX-controlled pixel maps to physical index 1.
constexpr uint16_t RESERVED_PIXEL_INDEX = 0;
// Low-level RGB for the taped-over pixel (tweak if desired)
constexpr uint8_t RESERVED_PIXEL_R = 30;
constexpr uint8_t RESERVED_PIXEL_G = 30;
constexpr uint8_t RESERVED_PIXEL_B = 30;

// safe ceiling for buffer allocation
#ifndef MAX_PIXELS
#define MAX_PIXELS 170
#endif

// ---------- Runtime (storage in config.cpp) ----------
extern const char *WIFI_SSID;
extern const char *WIFI_PASSWORD;

extern uint16_t E131_UNIVERSE;
extern uint16_t START_ADDR; // DMX start address (1..512)
extern bool USE_UNICAST;

extern int NUM_PIXELS; // DMX-addressable pixel count (physical pixels = NUM_PIXELS + 1)
extern uint8_t DEFAULT_BRIGHTNESS_CAP;
extern uint32_t DMX_TIMEOUT_MS;

// Static IP tuple
extern bool USE_STATIC_IP;
extern IPAddress STATIC_IP;
extern IPAddress STATIC_GW;
extern IPAddress STATIC_SN;
extern IPAddress STATIC_DNS;

// Run mode
enum RunMode : uint8_t
{
    MODE_4CH = 0,     // I R G B control the whole strip
    MODE_PERPIXEL = 1 // I + N×RGB per pixel
};
extern uint8_t RUN_MODE;
