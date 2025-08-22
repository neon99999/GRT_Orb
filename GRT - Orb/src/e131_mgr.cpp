#include "e131_mgr.h"
#include "config.h"

static ESPAsyncE131 e131(1);

void e131Init(){
  e131.begin(USE_UNICAST ? E131_UNICAST : E131_MULTICAST, E131_UNIVERSE);
  Serial.println(USE_UNICAST ? "sACN: unicast" : "sACN: multicast");
  Serial.print("Universe: "); Serial.println(E131_UNIVERSE);
  Serial.print("DMX start addr: "); Serial.println(START_ADDR);
}

bool e131Poll(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& w, uint8_t& dim, uint8_t& seq){
  if (e131.isEmpty()) return false;

  e131_packet_t p;
  e131.pull(&p);
  uint8_t* dmx = p.property_values + 1; // skip start code
  uint16_t s = START_ADDR ? START_ADDR - 1 : 0;

  r = g = b = w = dim = 0;
  if (s + 4 < 512){
    r   = dmx[s + 0];
    g   = dmx[s + 1];
    b   = dmx[s + 2];
    w   = dmx[s + 3];
    dim = dmx[s + 4];
  }
  seq = p.sequence_number;
  return true;
}
