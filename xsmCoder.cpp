#include "xsmCoder.h"

#include "xsmCRC.h"
#include "xsmUtils.h"

using namespace xsm;

Frame Coder::encode(const Payload& payload) {
  Frame frame;
  Utils::escape(payload, mEscapedPayload);
  if (mEscapedPayload.DataSize > 0) {
    frame.setEscapedPayload(mEscapedPayload.Data);
    frame.setPayloadSize(static_cast<uint8_t>(mEscapedPayload.DataSize));
    frame.setHeaderCrc(crc8(frame.getData().data(), HEADER_SIZE - 1));
    frame.setPayloadCrc(crc8(payload.Data.data(), payload.DataSize));
  }
  return frame;
}

void Coder::encode(const Payload& payload, PacketRaw& packet) {
  // perform escaping in member buffer. this might makes the payload bigger
  Utils::escape(payload, mEscapedPayload);

  // now mEscapeHelperBuffer holds the escaped payload
  // assemble the packet and return its size.
  if (mEscapedPayload.DataSize > 0) {
    assemble(mEscapedPayload, packet);
  }
}


void Coder::assemble(const Payload& escapedPayload, PacketRaw& encodedPacket) {
  // assemble the header
  size_t headerIndex = 0;
  // add frame delimiter first
  encodedPacket.Data[headerIndex++] = FRAME_DELIMITER;
  // add payload length
  encodedPacket.Data[headerIndex++] = static_cast<uint8_t>(escapedPayload.DataSize);
  // add header crc
  encodedPacket.Data[headerIndex++] = crc8(encodedPacket.Data.data(), HEADER_SIZE - 1);

  // copy header and escaped payload to the packet buffer
  std::copy(escapedPayload.Data.begin(),
            escapedPayload.Data.begin() + escapedPayload.DataSize,
            encodedPacket.Data.data() + HEADER_SIZE);

  // add footer: payload crc
  encodedPacket.Data[HEADER_SIZE + escapedPayload.DataSize] = crc8(escapedPayload.Data.data(), escapedPayload.DataSize);

  encodedPacket.DataSize = HEADER_SIZE + escapedPayload.DataSize + FOOTER_SIZE;
}
