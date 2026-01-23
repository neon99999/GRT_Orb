#pragma once
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>

// ---------- Compile-time hardware ----------
constexpr int LED_PIN = 5;
constexpr int STATUS_LED = 2;
#define PIXEL_ORDER (NEO_GRB + NEO_KHZ800)

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

extern int NUM_PIXELS; // strip length
extern uint8_t DEFAULT_BRIGHTNESS_CAP;
extern uint32_t DMX_TIMEOUT_MS;

// Static IP tuple
extern bool USE_STATIC_IP;
extern IPAddress STATIC_IP;
extern IPAddress STATIC_GW;
extern IPAddress STATIC_SN;
extern IPAddress STATIC_DNS;

// Idle / keepalive
extern bool KEEPALIVE_ON_TIMEOUT;
extern uint8_t KEEPALIVE_LEVEL;
extern uint8_t KEEPALIVE_PIXEL;
extern uint32_t KEEPALIVE_PULSE_MS;
extern uint32_t KEEPALIVE_PERIOD_MS;
extern uint16_t IDLE_TIMEOUT_MS;
extern uint8_t MIN_IDLE_MASTER;

// Run mode
enum RunMode : uint8_t
{
    MODE_4CH = 0,     // I R G B control the whole strip
    MODE_PERPIXEL = 1 // I + N×RGB per pixel
};
extern uint8_t RUN_MODE;
