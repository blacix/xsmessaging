#ifndef _XSM_CODEC
#define _XSM_CODEC

#include <array>
#include <cstdint>
#include <functional>

#include "xsmTypes.h"
#include "xsmFrame.h"

namespace xsm {

class Decoder {

public:
  enum class State { DELIMITER, SIZE, CRC_HEADER, PAYLOAD, CRC_PAYLOAD };

  Decoder(IFrameCallback& callback);
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
  IFrameCallback& mCallback;
  uint8_t mPrevByte;
  unsigned long long mDiscardedBytes;

  // preallocated buffer for incoming payload
  Message mPotentialPayload;
  // preallocated buffer for incoming header
  // HeaderBuffer mHeader;
  Frame mFrame;
  uint8_t mPayloadCrc;
};


} // namespace xsm

#endif
