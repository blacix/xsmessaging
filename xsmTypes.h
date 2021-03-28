#ifndef XSM_TYPES_H
#define XSM_TYPES_H
#include <array>
#include "xsmDefines.h"
#include "xsmRingBuffer.h"


namespace xsm {

// fix sized buffer for packets
typedef std::array<uint8_t, MAX_PACKET_SIZE> PacketBuffer;
typedef std::array<uint8_t, MAX_PAYLOAD_SIZE> PayloadBuffer;
// buffer for packet header
typedef std::array<uint8_t, HEADER_SIZE> HeaderBuffer;
// ringbuffer for incoming data
typedef RingBufferT<INPUT_BUFFER_SIZE> RingBuffer;

struct Payload {
  PayloadBuffer Data;
  size_t DataSize;
};

struct Packet {
  PacketBuffer Data;
  size_t DataSize;
};


} // namespace xsm

#endif