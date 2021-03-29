#ifndef _XSM_CODEC
#define _XSM_CODEC

#include <array>
#include <cstdint>
#include <functional>

#include "xsmTypes.h"

namespace xsm {

class Decoder {

public:

  enum class State {
    DELIMITER,
    SIZE,
    CRC_HEADER,
    PAYLOAD,
    CRC_PAYLOAD
  };

  Decoder(MessageCallback callback);
  ~Decoder() = default;

  void receive(const uint8_t byte);

private:

  void receiveDelimiter(const uint8_t byte);
  void receiveSize(const uint8_t byte);
  void receiveHeaderCrc(const uint8_t byte);
  void receivePayload(const uint8_t byte);
  void receivePayloadCrc(const uint8_t byte);

  void payloadToCrcPayload(const uint8_t byte);
  void reset();

  State mState;
  std::function<void(Message)> mCallback;
  uint8_t mPrevByte;
  unsigned long long mDiscardedBytes;

  // preallocated helper buffer for unescaping
  Message mUnescapedPayload;
  // preallocated buffer for incoming payload
  Message mPotentialPayload;
  // preallocated buffer for incoming header
  HeaderBuffer mHeader;
  

  
};


} // namespace xsm

#endif
