#ifndef XSM_CODER_H
#define XSM_CODER_H
#include "xsmTypes.h"

namespace xsm {
class xsmCoder {

public:
  // Encodes the payload provided as input into encodedBuffer that is the output.
  // First it escapes the necessary characters in the payload than assembles the packet
  size_t encode(const PayloadBuffer& unEscapedPayload, const size_t unEscapedPayloadSize, PacketBuffer& encodedData);

private:
  // Helper function that takes an escapedPayload as input and assembles the packet
  // that meets the requirements of the protocol into encodedPayload that is the output.
  // returns the size of the assembled packet or 0 on error.
  static size_t assemble(const PayloadBuffer& escapedPayload,
                         const size_t escapedPayloadSize,
                         PacketBuffer& encodedPacket);


  // preallocated helper buffer for escaping
  PayloadBuffer mEscapeHelperBuffer;

};
} // namespace xsm

#endif // !XSM_CODER_H