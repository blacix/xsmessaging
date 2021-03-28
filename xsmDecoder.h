#ifndef _XSM_CODEC
#define _XSM_CODEC

#include <array>
#include <cstdint>
#include <vector>
#include <functional>

#include "xsmTypes.h"
#include "xsmRingBuffer.h"

namespace xsm {

class Decoder {

public:

  enum class State {
    DELIMITER,
    SIZE,
    CRC,
    PAYLOAD,
    PAYLOAD_CRC
  };

  Decoder(std::function<void(Message)> callback);
  ~Decoder() = default;

  void receive(const uint8_t b);

  // Decodes every packet from the encodedData buffer provided as input
  // into the decodedPackets array that is the output
  // returns the number of bytes processed in the encodedPayload input buffer
  // thus the caller can remove processed bytes
  size_t decode(const RingBuffer& encodedFrames, std::vector<Message>& decodedMessages);


private:

  State mState;
  std::function<void(Message)> mCallback;
  // preallocated helper buffer for unescaping
  Message mUnescapedPayload;
  // preallocated buffer for incoming payload
  Message mPotentialPayload;
  // preallocated buffer for incoming header
  HeaderBuffer mHeader;

  uint8_t mPrevByte = FRAME_DELIMITER;

  unsigned long long mDiscardedBytes = 0;

  
};


} // namespace xsm

#endif