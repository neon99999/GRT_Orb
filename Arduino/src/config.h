#pragma once
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// provided in config.cpp
extern const char WIFI_SSID[];
extern const char WIFI_PASSWORD[];

// sACN mapping
constexpr uint16_t E131_UNIVERSE = 1;
constexpr uint16_t START_ADDR = 321; // I R G B W at 321..325
constexpr bool USE_UNICAST = true;

// hardware
constexpr int LED_PIN = 5;
constexpr int NUM_PIXELS = 12;
constexpr int STATUS_LED = 2;

// RGBW order for Adafruit 12-ring
#define PIXEL_ORDER (NEO_GRBW + NEO_KHZ800)

// safety and behavior
constexpr uint8_t DEFAULT_BRIGHTNESS_CAP = 155;
constexpr uint32_t DMX_TIMEOUT_MS = 3000;

// IP tuple
// Theatre router example
// constexpr bool USE_STATIC_IP = true;
// const IPAddress STATIC_IP (10,10,50,201);
// const IPAddress STATIC_GW (10,10,50,1);
// const IPAddress STATIC_SN (255,255,255,0);
// const IPAddress STATIC_DNS(10,10,50,1);

// Home testing example
constexpr bool USE_STATIC_IP = true;
const IPAddress STATIC_IP(192, 168, 1, 99);
const IPAddress STATIC_GW(192, 168, 1, 1);
const IPAddress STATIC_SN(255, 255, 255, 0);
const IPAddress STATIC_DNS(192, 168, 1, 1);

// keep-alive to prevent power bank sleep
constexpr bool KEEPALIVE_ON_TIMEOUT = true;
constexpr uint8_t KEEPALIVE_LEVEL = 2; // 0..255
constexpr uint8_t KEEPALIVE_PIXEL = 0; // index
