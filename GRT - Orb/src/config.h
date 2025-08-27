#pragma once
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// define once in config.cpp
extern const char WIFI_SSID[];
extern const char WIFI_PASSWORD[];

// compile-time settings
constexpr uint16_t E131_UNIVERSE = 1;
constexpr uint16_t START_ADDR    = 1;   // R,G,B,W,Dim
constexpr bool     USE_UNICAST   = true;

constexpr int LED_PIN    = 5;
constexpr int NUM_PIXELS = 12;
constexpr int STATUS_LED = 2;

// your ring is RGBW; switch to NEO_RGBW if colors are wrong
#define PIXEL_ORDER (NEO_GRBW + NEO_KHZ800)

constexpr uint8_t  DEFAULT_BRIGHTNESS_CAP = 155;
constexpr uint32_t DMX_TIMEOUT_MS         = 3000;

constexpr bool USE_STATIC_IP = true;
const IPAddress STATIC_IP (10,10,50,201);   // change per unit
const IPAddress STATIC_GW (10,10,50,1);
const IPAddress STATIC_SN (255,255,255,0);
const IPAddress STATIC_DNS(10,10,50,1);
