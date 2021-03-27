#include "xsmCoder.h"

#include "CRCMaximDallas.h"
#include "xsmUtils.h"

using namespace xsm;


size_t xsmCoder::encode(const PayloadBuffer& unEscapedPayload,
                        const size_t unEscapedPayloadSize,
                        PacketBuffer& encodedData) {
  // perform escaping in member buffer. this might makes the payload bigger
  size_t escapedPayloadBufferSize = xsmUtils::escape(unEscapedPayload, unEscapedPayloadSize, mEscapeHelperBuffer);

  // now mEscapeHelperBuffer holds the escaped payload
  // assemble the packet and return its size.
  if (escapedPayloadBufferSize > 0)
    return assemble(mEscapeHelperBuffer, escapedPayloadBufferSize, encodedData);
  else
    return 0;
}

size_t xsmCoder::assemble(const PayloadBuffer& escapedPayload,
                          const size_t escapedPayloadSize,
                          PacketBuffer& encodedPacket) {
  // if the escaped data provided is too large, it won't fit in a max size packet
  // this is checked outside as well, but this being a public static method
  // it is better to make sure noone calls it with a wrong parameter
  if (escapedPayloadSize > MAX_PAYLOAD_SIZE)
    return 0;

  // assemble the header
  size_t headerIndex = 0;
  // add frame delimiter first
  encodedPacket[headerIndex++] = FRAME_DELIMITER;
  // add payload length
  encodedPacket[headerIndex++] = static_cast<uint8_t>(escapedPayloadSize);
  // add header crc
  encodedPacket[headerIndex++] = crc8MaximDallas(encodedPacket.data(), 2);

  // copy header and escaped payload to the packet buffer
  size_t payloadIndex = 0;
  for (; payloadIndex < escapedPayloadSize; payloadIndex++) {
    encodedPacket[headerIndex + payloadIndex] = escapedPayload[payloadIndex];
  }

  // post incremented indexes become sizes

  // add footer: payload crc
  encodedPacket[headerIndex + payloadIndex] = crc8MaximDallas(escapedPayload.data(), escapedPayloadSize);


  // std::stringstream ss;
  // ss << std::hex;
  // for (int i = 0; i < headerIndex + payloadIndex + FOOTER_SIZE; i++)
  //{
  //	ss << "0x" << (int)encodedData[i] << " ";
  //}
  // std::cout << ss.str() << std::endl;;

  // return the size of the assembled packet
  return headerIndex + payloadIndex + FOOTER_SIZE;
}
