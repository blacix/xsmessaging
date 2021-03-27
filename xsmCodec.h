#ifndef _XSM_CODEC
#define _XSM_CODEC

#include <array>
#include <cstdint>
#include <vector>

#include "xsmTypes.h"

namespace xsm {

class xsmCodec {

public:
  xsmCodec() = default;
  ~xsmCodec() = default;
  // Encodes the payload provided as input into encodedBuffer that is the output.
  // First it escapes the necessary characters in the payload than assembles the packet
  size_t encode(const PayloadBuffer& unEscapedPayload, const size_t unEscapedPayloadSize, PacketBuffer& encodedData);

  // Decodes every packet from the encodedData buffer provided as input
  // into the decodedPackets array that is the output
  // returns the number of bytes processed in the encodedPayload input buffer
  // thus the caller can remove processed bytes
  size_t decode(const RingBuffer& encodedPackets, std::vector<PacketBuffer>& decodedPackets);


private:



  // Helper function that takes an escapedPayload as input and assembles the packet
  // that meets the requirements of the protocol into encodedPayload that is the output.
  // returns the size of the assembled packet or 0 on error.
  static size_t assemble(const PayloadBuffer& escapedPayload,
                         const size_t escapedPayloadSize,
                         PacketBuffer& encodedPacket);


  // preallocated helper buffer for escaping
  PayloadBuffer mEscapeHelperBuffer;
  // preallocated buffer for incoming payload
  PayloadBuffer mPotentialPayload;
  // preallocated buffer for incoming header
  HeaderBuffer mHeader;

  unsigned long long mDiscardedBytes = 0;
};


} // namespace xsm

#endif