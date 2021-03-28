#ifndef _XSM_CODEC
#define _XSM_CODEC

#include <array>
#include <cstdint>
#include <vector>

#include "xsmTypes.h"
#include "xsmRingBuffer.h"

namespace xsm {

class Decoder {

public:
  Decoder() = default;
  ~Decoder() = default;


  // Decodes every packet from the encodedData buffer provided as input
  // into the decodedPackets array that is the output
  // returns the number of bytes processed in the encodedPayload input buffer
  // thus the caller can remove processed bytes
  size_t decode(const RingBuffer& encodedFrames, std::vector<Message>& decodedMessages);


private:

  // preallocated helper buffer for unescaping
  Message mUnescapedPayload;
  // preallocated buffer for incoming payload
  Message mPotentialPayload;
  // preallocated buffer for incoming header
  HeaderBuffer mHeader;

  unsigned long long mDiscardedBytes = 0;
};


} // namespace xsm

#endif