#include "xsmCodec.h"

using namespace xsm;


size_t xsmCodec::encode(const PayloadBuffer& unEscapedPayload,
                        const size_t unEscapedPayloadSize,
                        PacketBuffer& encodedData) {
  // perform escaping in member buffer. this might makes the payload bigger
  size_t escapedPayloadBufferSize = escape(unEscapedPayload, unEscapedPayloadSize, mEscapeHelperBuffer);
  
  // now mEscapeHelperBuffer holds the escaped payload
  // assemble the packet and return its size.
  if (escapedPayloadBufferSize > 0)
    return assemble(mEscapeHelperBuffer, escapedPayloadBufferSize, encodedData);
  else
    return 0;
}

// The ProtocolConfig::decode function uses a naive approach, that is,
// it always starts interpreting the input buffer from the beginning.This has, of course, impact on the performance.
size_t xsmCodec::decode(const RingBuffer& encodedPackets, std::vector<PacketBuffer>& decodedPackets) {
  // number of bytes processed in the
  size_t bytesProcessed = 0;
  // due to escaping the previusly processed byte need to be tracked
  uint8_t prevByte = 0;
  // tha index of the packet delimiter need to be tracked to be able to discard corrupted data
  size_t packetDelimiterIndex = -1;
  // helper variable for getting values from the ringbuffer
  uint8_t valueAtIndex = 0;

  // iterate through the input ringbuffer searching for valid packets
  for (size_t i = 0; i < encodedPackets.capacity(); ++i) {
    prevByte = valueAtIndex;
    encodedPackets.get(i, valueAtIndex);
    // the first position is we need as there might be delimitier in the payload as well
    if (valueAtIndex == FRAME_DELIMITER && prevByte != ESCAPE_BYTE)
      packetDelimiterIndex = i;

    // check if the buffer contains enough data for a min size packet that is the header and a min 1 byte long payload
    if (encodedPackets.capacity() >= i + MIN_PACKET_SIZE) {
      encodedPackets.get(i, mHeader.data(), HEADER_SIZE);
      // every packet starts with the frame byte
      if (mHeader[0] == FRAME_DELIMITER) {
        // extract payload length
        uint8_t payloadLength = mHeader[PAYLOAD_LENGTH_INDEX];
        // check header crc
        if (crc8(mHeader.data(), HEADER_SIZE - 1) == mHeader[2]) {
          // check if we have enough data based on the payload length
          uint16_t packetLength = HEADER_SIZE + payloadLength + FOOTER_SIZE;
          if (encodedPackets.capacity() >= i + packetLength) {
            // extract payload

            // copy payload to mPotentialPayload buffer
            encodedPackets.get(i + HEADER_SIZE, mPotentialPayload.data(), payloadLength);

            // if there is unescaped delimiter in the payload that might be the start of a new packet
            // discard everything before it
            int delimiterIndex = unescapedDelimiterPos(mPotentialPayload, payloadLength);
            if (delimiterIndex > 0) {
              // discard all before this index
              bytesProcessed = i + HEADER_SIZE + delimiterIndex;
            } else {
              // check payload crc
              uint8_t payloadCrc = 0;
              encodedPackets.get(i + HEADER_SIZE + payloadLength, payloadCrc);
              if (crc8(mPotentialPayload.data(), payloadLength) == payloadCrc) {
                // remove escape characters
                unescape(mPotentialPayload, payloadLength, mEscapeHelperBuffer);
                // add it to the output array of payloads
                PacketBuffer packet;
                std::copy(mEscapeHelperBuffer.begin(), mEscapeHelperBuffer.end(), packet.begin());
                decodedPackets.push_back(packet);

                // move on to processing the next packet
                i += packetLength;
                bytesProcessed = i;
              }
            }
          }
        }
      }
    }
  }

  // buffer should be cleared if there were no packets found
  if (packetDelimiterIndex < 0) {
    // if no delimiter found, this must be a corrupted message
    // discard everything in the buffer
    bytesProcessed = encodedPackets.capacity();
    mDiscardedBytes += bytesProcessed;
  } else {
    // delimiter found
    if (decodedPackets.size() < 1) {
      // no packets found
      // bytes before the delimiter are to be discarded
      bytesProcessed = packetDelimiterIndex;
      mDiscardedBytes += bytesProcessed;
    }
  }

  return bytesProcessed;
}

// CRC for Maxim/Dallas 1-Wire
uint8_t xsmCodec::crc8(const uint8_t* buffer, const uint8_t size) {
  uint8_t crc = 0;
  uint8_t sum = 0;
  for (size_t i = 0; i < size; i++) {
    uint8_t byte = buffer[i];
    for (uint8_t tmp = 8; tmp; tmp--) {
      sum = (crc ^ byte) & 0x01;
      crc >>= 1;
      if (sum)
        crc ^= 0x8C;
      byte >>= 1;
    }
  }
  return crc;
}


size_t xsmCodec::escape(const PayloadBuffer& unescapedPayload,
                        const size_t unescapedPayloadSize,
                        PayloadBuffer& escapedPayload) {

  size_t escapedPayloadSize = 0;
  // iterate through the input payload buffer and insert escaped payload in the
  // escapedPayload output buffer
  for (size_t i = 0; i < unescapedPayloadSize; i++) {
    uint8_t b = unescapedPayload[i];
    // don't escape the frame delimiter at first position
    if (i != 0) {
      // the frame delimiter and the escape byte itself are to be escaped
      if (b == FRAME_DELIMITER || b == ESCAPE_BYTE) {
        escapedPayload[i] = ESCAPE_BYTE;
        ++escapedPayloadSize;
        if (escapedPayloadSize > unescapedPayloadSize) {
          return 0;
        }
      }
    }
    escapedPayload[escapedPayloadSize] = b;
    ++escapedPayloadSize;
  }

  return escapedPayloadSize;
}


size_t xsmCodec::assemble(const PayloadBuffer& escapedPayload, const size_t escapedPayloadSize, PacketBuffer& encodedPacket) {
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
  encodedPacket[headerIndex++] = crc8(encodedPacket.data(), 2);

  // copy header and escaped payload to the packet buffer
  size_t payloadIndex = 0;
  for (; payloadIndex < escapedPayloadSize; payloadIndex++) {
    encodedPacket[headerIndex + payloadIndex] = escapedPayload[payloadIndex];
  }

  // post incremented indexes become sizes

  // add footer: payload crc
  encodedPacket[headerIndex + payloadIndex] = crc8(escapedPayload.data(), escapedPayloadSize);



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


size_t xsmCodec::unescape(const PayloadBuffer& escapedPayload,
                          const size_t escapedPayloadSize,
                          PayloadBuffer& unEscapedPayload) {
  int skippedBytes = 0;
  // helper variable to remove the first escape character only.
  // this is the case when the escape charcter is escaped in the payload
  bool escaped = false;

  // iterate through escapedPayload buffer and remove escaped characters
  size_t unescapedPayloadSize = 0;
  for (size_t i = 0; i < escapedPayloadSize; i++) {
    if (escapedPayload[i] == ESCAPE_BYTE) {
      if (escaped) {
        // prev byte was the escape character
        // this byte is also the escape character
        escaped = false;
        // skip this byte
      } else {
        // prev byte was not the escape character
        escaped = true;
        unEscapedPayload[unescapedPayloadSize] = escapedPayload[i];
        ++unescapedPayloadSize;
      }
    } else {
      unEscapedPayload[unescapedPayloadSize] = escapedPayload[i];
      ++unescapedPayloadSize;
    }    
  }

  return unescapedPayloadSize;
}

int xsmCodec::unescapedDelimiterPos(const PayloadBuffer& buffer, const size_t bufferSize) {
  uint8_t prevByte = 0;
  // iterate through the buffer
  for (size_t i = 0; i < bufferSize; i++) {
    if (buffer[i] == FRAME_DELIMITER && prevByte != ESCAPE_BYTE) {
      // index found, return it
      return i;
    }
    prevByte = buffer[i];
  }

  return -1;
}
