#pragma once
#include <Arduino.h>
#include <ESPAsyncE131.h>

void e131Init();
bool e131Poll(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& w, uint8_t& dim, uint8_t& seq);
