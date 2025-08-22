#pragma once
#include <Arduino.h>
#include <WiFi.h>

void wifiInit();
void wifiEnsure();
IPAddress wifiIP();
void wifiPrintStatus();
