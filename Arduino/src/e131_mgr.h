#pragma once
#include <Arduino.h>
#include <ESPAsyncE131.h>

// init socket
void e131Init();

// raw poll: copies DMX payload (512 bytes, start code skipped) to outBuf
// returns true if a packet was pulled
bool e131PollRaw(uint8_t* outBuf, uint16_t& outLen, uint8_t& seq);
