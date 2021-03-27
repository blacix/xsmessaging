#ifndef _XSM_CODEC
#define _XSM_CODEC

// project includes
#include "xsmRingBuffer.h"
// library includes
#include <array>
#include <cstdint>
#include <vector>


namespace xsm {


// packet delimiter byte
// 0xab = 802.3 Start Frame Delimiter
static const uint8_t FRAME_DELIMITER = 0b10101011;
// escape byte
// 0x2f = '/' ascii char
static const uint8_t ESCAPE_BYTE = 0b00101111;
// payload length position in header
static const uint8_t PAYLOAD_LENGTH_INDEX = 1;
static const uint8_t HEADER_SIZE = 3;
static const uint8_t FOOTER_SIZE = 1;
static const uint8_t MIN_PAYLOAD_SIZE = 1;
static const uint8_t MAX_PAYLOAD_SIZE = 255;
static const uint16_t MAX_PACKET_SIZE = HEADER_SIZE + MAX_PAYLOAD_SIZE + FOOTER_SIZE;
static const uint16_t MIN_PACKET_SIZE = HEADER_SIZE + MIN_PAYLOAD_SIZE + FOOTER_SIZE;
static const size_t INPUT_BUFFER_SIZE = 2048;

// fix sized buffer for packets
typedef std::array<uint8_t, MAX_PACKET_SIZE> PacketBuffer;
// buffer for packet header
typedef std::array<uint8_t, HEADER_SIZE> HeaderBuffer;
// ringbuffer for incoming data
typedef xsmRingBuffer<INPUT_BUFFER_SIZE> RingBuffer;

class xsmCodec {

public:
  xsmCodec();
  ~xsmCodec() = default;

  // Encodes the payload provided as input into encodedBuffer that is the output.
  // First it escapes the necessary characters in the payload than assembles the packet
  size_t encode(const std::vector<uint8_t>& payload, PacketBuffer& encodedData);

  // Decodes every packet from the encodedData buffer provided as input
  // into the decodedPackets array that is the output
  // returns the number of bytes processed in the encodedPayload input buffer
  // thus the caller can remove processed bytes
  size_t decode(const RingBuffer& encodedPackets, std::vector<PacketBuffer>& decodedPackets);

  // a simple crc8 implementation
  static uint8_t crc8(const uint8_t* buffer, size_t size);

private:
  // Function to check for characters in the payload that are to be escaped.
  // These are the Frame Delimiter and the escape character itself.
  static void escape(const std::vector<uint8_t>& payload, std::vector<uint8_t>& escapedPayload);

  // Helper function that takes an escapedPayload as input and assembles the packet
  // that meets the requirements of the protocol into encodedPayload that is the output.
  // returns the size of the assembled packet or 0 on error.
  // The method also demonstartes how using a fixed size array is more difficult than using a dynamic one.
  // With preallocated std vectors the same functionality is easier to achieve
  // if heap usage is not an issue.
  static size_t assemble(const std::vector<uint8_t>& escapedPayload, PacketBuffer& encodedPayload);

  // Helper function to remove escape characters from an escaped payload
  // returns the number of escae bytes removed from the buffer
  static size_t unescape(std::vector<uint8_t>& escapedPayload);

  // helper method that searches for unescaped delimiter in the buffer
  static int unescapedDelimiterPos(const std::vector<uint8_t>& buffer);

  // preallocated helper buffer for escaping
  std::vector<uint8_t> mEscapeHelperBuffer;
  // preallocated buffer for incoming payload
  std::vector<uint8_t> mPotentialPayload;
  // preallocated buffer for incoming header
  HeaderBuffer mHeader;

  unsigned long long mDiscardedBytes = 0;
};


} // namespace xsm

#endif