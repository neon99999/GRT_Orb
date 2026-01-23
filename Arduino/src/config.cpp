#include "config.h"

// pick profile here
enum Profile
{
  Studio,
  Theatre
};
static Profile ACTIVE_PROFILE = Studio;

// Declarations
const char *WIFI_SSID;
const char *WIFI_PASSWORD;

uint16_t E131_UNIVERSE;
uint16_t START_ADDR;
bool USE_UNICAST;

int NUM_PIXELS;

uint8_t DEFAULT_BRIGHTNESS_CAP;
uint32_t DMX_TIMEOUT_MS;

bool USE_STATIC_IP;
IPAddress STATIC_IP, STATIC_GW, STATIC_SN, STATIC_DNS;

bool KEEPALIVE_ON_TIMEOUT;
uint8_t KEEPALIVE_LEVEL, KEEPALIVE_PIXEL;
uint32_t KEEPALIVE_PULSE_MS, KEEPALIVE_PERIOD_MS;
uint16_t IDLE_TIMEOUT_MS = 3000;
uint8_t MIN_IDLE_MASTER = 0;

uint8_t RUN_MODE = MODE_PERPIXEL;

// ---------- Loaders ----------
static void loadStudio()
{
  WIFI_SSID = "flubbyLindo";
  WIFI_PASSWORD = "LaIslaSol2022!";

  E131_UNIVERSE = 1;
  START_ADDR = 321;
  USE_UNICAST = true;

  NUM_PIXELS = 25;
  DEFAULT_BRIGHTNESS_CAP = 200;

  DMX_TIMEOUT_MS = 3000;
  KEEPALIVE_ON_TIMEOUT = true;
  KEEPALIVE_LEVEL = 100;
  KEEPALIVE_PIXEL = 0;
  KEEPALIVE_PULSE_MS = 40;
  KEEPALIVE_PERIOD_MS = 1500;

  RUN_MODE = MODE_PERPIXEL;

  USE_STATIC_IP = true;
  STATIC_IP = IPAddress(192, 168, 1, 99);
  STATIC_GW = IPAddress(192, 168, 1, 1);
  STATIC_SN = IPAddress(255, 255, 255, 0);
  STATIC_DNS = IPAddress(192, 168, 1, 1);
}

static void loadTheatre()
{
  WIFI_SSID = "ORB";
  WIFI_PASSWORD = "orbAdmin2025";

  E131_UNIVERSE = 1;
  START_ADDR = 321;
  USE_UNICAST = true;

  NUM_PIXELS = 25;
  DEFAULT_BRIGHTNESS_CAP = 180;

  DMX_TIMEOUT_MS = 3000;
  KEEPALIVE_ON_TIMEOUT = false;
  KEEPALIVE_LEVEL = 3;
  KEEPALIVE_PIXEL = 0;
  KEEPALIVE_PULSE_MS = 30;
  KEEPALIVE_PERIOD_MS = 1500;

  RUN_MODE = MODE_PERPIXEL;

  USE_STATIC_IP = true;
  STATIC_IP = IPAddress(10, 10, 50, 201);
  STATIC_GW = IPAddress(10, 10, 50, 1);
  STATIC_SN = IPAddress(255, 255, 255, 0);
  STATIC_DNS = IPAddress(10, 10, 50, 1);
}


  // Auto-load
  __attribute__((constructor)) static void loadProfile()
  {
    if (ACTIVE_PROFILE == Theatre)
      loadTheatre();
    else
      loadStudio();
  }
