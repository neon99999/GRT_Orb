#include "wifi_mgr.h"
#include "config.h"

static bool ledState = false;
static uint32_t nextReconnectMs = 0;
static uint32_t backoffMs = 1000;   // start 1 s, max 30 s

void wifiPrintStatus(){
  Serial.println();
  Serial.println("=== WiFi ===");
  Serial.print("SSID: "); Serial.println(WIFI_SSID);
  Serial.print("IP:   "); Serial.println(WiFi.localIP());
  Serial.print("GW:   "); Serial.println(WiFi.gatewayIP());
  Serial.print("RSSI: "); Serial.print(WiFi.RSSI()); Serial.println(" dBm");
  Serial.print("MAC:  "); Serial.println(WiFi.macAddress());
  Serial.println("===========");
}

void wifiInit(){
  pinMode(STATUS_LED, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.persistent(false);         // don't write creds every boot
  WiFi.setSleep(false);           // lower latency, steadier sACN

  if (USE_STATIC_IP)              // set static before begin()
    WiFi.config(STATIC_IP, STATIC_GW, STATIC_SN, STATIC_DNS);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println();
  Serial.println("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED){
    ledState = !ledState;
    digitalWrite(STATUS_LED, ledState);
    Serial.print(".");
    delay(300);
  }
  digitalWrite(STATUS_LED, HIGH);
  Serial.println("\nWiFi connected");
  wifiPrintStatus();
  backoffMs = 1000;
  nextReconnectMs = 0;
}


void wifiEnsure(){
  if (WiFi.status() == WL_CONNECTED) return;
  uint32_t now = millis();
  if (now < nextReconnectMs) return;

  Serial.println("WiFi lost, reconnecting");
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  if (USE_STATIC_IP) WiFi.config(STATIC_IP, STATIC_GW, STATIC_SN, STATIC_DNS);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  uint32_t t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 2000){
    ledState = !ledState;
    digitalWrite(STATUS_LED, ledState);
    delay(200);
  }
  if (WiFi.status() == WL_CONNECTED){
    digitalWrite(STATUS_LED, HIGH);
    Serial.println("WiFi reconnected");
    wifiPrintStatus();
    backoffMs = 1000;
    nextReconnectMs = 0;
  } else {
    backoffMs = min(backoffMs * 2, (uint32_t)30000);
    nextReconnectMs = now + backoffMs;
    Serial.print("Retry in "); Serial.print(backoffMs); Serial.println(" ms");
  }
}

IPAddress wifiIP(){ return WiFi.localIP(); }
