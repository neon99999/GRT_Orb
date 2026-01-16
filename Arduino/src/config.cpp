#include "config.h"

// --------- pick profile here ---------
enum Profile { Studio, Theatre };
static Profile ACTIVE_PROFILE = Theatre;   // <--- change to Theatre when needed
// -------------------------------------

// Declarations (storage)
const char* WIFI_SSID;
const char* WIFI_PASSWORD;

uint16_t E131_UNIVERSE;
uint16_t START_ADDR;
bool     USE_UNICAST;

int      NUM_PIXELS;

uint8_t  DEFAULT_BRIGHTNESS_CAP;
uint32_t DMX_TIMEOUT_MS;

bool      USE_STATIC_IP;
IPAddress STATIC_IP, STATIC_GW, STATIC_SN, STATIC_DNS;

bool     KEEPALIVE_ON_TIMEOUT;
uint8_t  KEEPALIVE_LEVEL, KEEPALIVE_PIXEL;
uint32_t KEEPALIVE_PULSE_MS, KEEPALIVE_PERIOD_MS;

uint16_t IDLE_TIMEOUT_MS = 3000; // 3 s of blackout
uint8_t  MIN_IDLE_MASTER = 0;    // set to 2..6 if you want a faint floor

// --------- loaders ---------
static void loadStudio(){
  // Wi-Fi
  WIFI_SSID     = "CuatroCabezas";
  WIFI_PASSWORD = "EDMLstudio2024!";

  // sACN
  E131_UNIVERSE = 1;
  START_ADDR    = 321;
  USE_UNICAST   = true;

  // LEDs
  NUM_PIXELS               = 25;
  DEFAULT_BRIGHTNESS_CAP   = 200;

  // Behavior
  DMX_TIMEOUT_MS           = 3000;
  KEEPALIVE_ON_TIMEOUT     = true;
  KEEPALIVE_LEVEL          = 100;
  KEEPALIVE_PIXEL          = 0;
  KEEPALIVE_PULSE_MS       = 40;
  KEEPALIVE_PERIOD_MS      = 1500;


  // IP
  USE_STATIC_IP = true;
  STATIC_IP     = IPAddress(192,168,1,99);
  STATIC_GW     = IPAddress(192,168,1,254);
  STATIC_SN     = IPAddress(255,255,255,0);
  STATIC_DNS    = IPAddress(192,168,1,254);
}

static void loadTheatre(){
  WIFI_SSID     = "ORB";
  WIFI_PASSWORD = "orbAdmin2025";

  E131_UNIVERSE = 1;
  START_ADDR    = 321;
  USE_UNICAST   = true;

  NUM_PIXELS               = 25;
  DEFAULT_BRIGHTNESS_CAP   = 220;

  DMX_TIMEOUT_MS           = 3000;
  KEEPALIVE_ON_TIMEOUT     = true;
  KEEPALIVE_LEVEL          = 2;
  KEEPALIVE_PIXEL          = 0;
  KEEPALIVE_PULSE_MS       = 40;
  KEEPALIVE_PERIOD_MS      = 1500;



  USE_STATIC_IP = true;
  STATIC_IP     = IPAddress(10,10,50,201);
  STATIC_GW     = IPAddress(10,10,50,1);
  STATIC_SN     = IPAddress(255,255,255,0);
  STATIC_DNS    = IPAddress(10,10,50,1);
}

// Auto-load at startup before setup()
__attribute__((constructor))
static void loadProfile(){
  if (ACTIVE_PROFILE == Theatre) loadTheatre();
  else                           loadStudio();
}