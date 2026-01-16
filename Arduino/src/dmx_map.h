#pragma once
#include <Arduino.h>

struct DmxFrameView {
  const uint8_t* dmx;   // pointer to DMX payload (start code skipped)
  uint16_t       len;   // expected 512
  uint16_t       start; // START_ADDR - 1, clamped to 0
};

// fills per-pixel RGB buffer and reads global intensity
// also checks optional broadcast triplet right after the pixel block
// returns false if not enough channels are present
bool dmxMap_PerPixelRGB_GlobalI(const DmxFrameView& f,
                                uint8_t* rgbBuf, uint16_t pixels,
                                uint8_t& globalI,
                                bool& useBroadcast,
                                uint8_t& br, uint8_t& bg, uint8_t& bb);
