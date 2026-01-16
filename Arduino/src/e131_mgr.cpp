#include "e131_mgr.h"
#include "config.h"

static ESPAsyncE131 e131(1);

void e131Init(){
  e131.begin(USE_UNICAST ? E131_UNICAST : E131_MULTICAST, E131_UNIVERSE);
  Serial.println(USE_UNICAST ? "sACN: unicast" : "sACN: multicast");
  Serial.print("Universe: "); Serial.println(E131_UNIVERSE);
}

bool e131PollRaw(uint8_t* outBuf, uint16_t& outLen, uint8_t& seq){
  if (e131.isEmpty()) return false;

  e131_packet_t p;
  e131.pull(&p);

  // DMX payload after start code
  const uint8_t* dmx = p.property_values + 1;
  // packet length is always up to 512 for DMX512
  outLen = 512;
  memcpy(outBuf, dmx, outLen);

  seq = p.sequence_number;
  return true;
}
