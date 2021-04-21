#ifndef XSM_TYPES_H
#define XSM_TYPES_H
#include <array>
#include <functional>

#include "xsmDefines.h"
#include "xsmRingBuffer.h"


namespace xsm {

// fix sized buffer for packets
typedef std::array<uint8_t, MAX_FRAME_SIZE> FrameBuffer;
typedef std::array<uint8_t, MAX_PAYLOAD_SIZE> MessageBuffer;
// buffer for packet header
typedef std::array<uint8_t, HEADER_SIZE> HeaderBuffer;
// ringbuffer for incoming data
typedef RingBufferT<INPUT_BUFFER_SIZE> RingBuffer;

struct Message {
  MessageBuffer Data;
  uint32_t Size = 0;
};

enum class Escaping {
  ON,
  OFF,
};

class IMessageCallback {
public:
  virtual void onMessageReceived(const Message) = 0;
};


} // namespace xsm

#endif
