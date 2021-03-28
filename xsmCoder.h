#ifndef XSM_CODER_H
#define XSM_CODER_H
#include "xsmFrame.h"
#include "xsmTypes.h"

namespace xsm {

class Coder {

public:
  Frame encode(const Message& payload);

  // Encodes the payload provided as input into encodedBuffer that is the output.
  // First it escapes the necessary characters in the payload than assembles the packet
  // void encode(const Message& payload, PacketRaw& packet);


private:
  // Helper function that takes an escapedPayload as input and assembles the packet
  // that meets the requirements of the protocol into encodedPayload that is the output.
  // returns the size of the assembled packet or 0 on error.
  // static void assemble(const Message& escapedPayload, PacketRaw& encodedPacket);


  // preallocated helper buffer for escaping
  Message mEscapedPayload;
};

} // namespace xsm

#endif // !XSM_CODER_H