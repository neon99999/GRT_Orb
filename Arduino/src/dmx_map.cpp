// dmx_map.cpp
#include "dmx_map.h"

bool dmxMap_PerPixelRGB_GlobalI(const DmxFrameView &f,
                                uint8_t *rgbBuf, uint16_t pixels,
                                uint8_t &globalI,
                                bool &useBroadcast,
                                uint8_t &br, uint8_t &bg, uint8_t &bb)
{
  useBroadcast = false;
  br = bg = bb = 0;
  globalI = 255;

  if (!f.dmx || f.len < 1)
    return false;
  uint16_t s = f.start;

  if (s >= f.len)
    return false;

  // global intensity at N
  globalI = f.dmx[s + 0];

  // 1 + 3*pixels needed for I + RGBRGB...
  uint32_t need = 1u + 3u * pixels;
  if (s + need > f.len)
    return false;

  const uint8_t *p = f.dmx + s + 1;
  for (uint16_t i = 0; i < pixels; i++)
  {
    rgbBuf[3 * i + 0] = p[3 * i + 0];
    rgbBuf[3 * i + 1] = p[3 * i + 1];
    rgbBuf[3 * i + 2] = p[3 * i + 2];
  }

  // optional broadcast triplet after the block
  if (s + need + 3 <= f.len)
  {
    br = f.dmx[s + need + 0];
    bg = f.dmx[s + need + 1];
    bb = f.dmx[s + need + 2];
    useBroadcast = (br | bg | bb) != 0;
  }

  return true;
}
