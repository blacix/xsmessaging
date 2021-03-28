#include "xsmCoder.h"

#include "CRCMaximDallas.h"
#include "xsmUtils.h"

using namespace xsm;


void Coder::encode(const Payload& payload, Packet& packet) {
  // perform escaping in member buffer. this might makes the payload bigger
  Utils::escape(payload, mEscapedPayload);

  // now mEscapeHelperBuffer holds the escaped payload
  // assemble the packet and return its size.
  if (mEscapedPayload.DataSize > 0) {
    assemble(mEscapedPayload, packet);
  }
}

void Coder::assemble(const Payload& escapedPayload, Packet& encodedPacket) {
  // assemble the header
  size_t headerIndex = 0;
  // add frame delimiter first
  encodedPacket.Data[headerIndex++] = FRAME_DELIMITER;
  // add payload length
  encodedPacket.Data[headerIndex++] = static_cast<uint8_t>(escapedPayload.DataSize);
  // add header crc
  encodedPacket.Data[headerIndex++] = crc8MaximDallas(encodedPacket.Data.data(), 2);

  // copy header and escaped payload to the packet buffer
  std::copy(escapedPayload.Data.begin(),
            escapedPayload.Data.begin() + escapedPayload.DataSize,
            encodedPacket.Data.data() + HEADER_SIZE);

  // add footer: payload crc
  encodedPacket.Data[HEADER_SIZE + escapedPayload.DataSize]
      = crc8MaximDallas(escapedPayload.Data.data(), escapedPayload.DataSize);

  encodedPacket.DataSize = HEADER_SIZE + escapedPayload.DataSize + FOOTER_SIZE;
}
