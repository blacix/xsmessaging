#ifndef _XSM_CODEC
#define _XSM_CODEC

#include <array>
#include <cstdint>
#include <functional>

#include "xsmFrame.h"
#include "xsmIReceiver.h"
#include "xsmTypes.h"

namespace xsm {

class Receiver {

public:
  enum class State { DELIMITER, SIZE, CRC_HEADER, PAYLOAD, CRC_PAYLOAD };

  Receiver(IReceiver& callback);
  ~Receiver() = default;

  void receive(const uint8_t byte);

private:
  void receiveDelimiter(const uint8_t byte);
  void receiveSize(const uint8_t byte);
  void receiveHeaderCrc(const uint8_t byte);
  void receivePayload(const uint8_t byte);
  void receivePayloadCrc(const uint8_t byte);

  void payloadToCrcPayload(const uint8_t byte);
  void reset();

  void processFrame();

  IReceiver& mCallback;

  State mState;
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
