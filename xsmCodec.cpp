#include "xsmCodec.h"

// project includes
#include "xsmCodec.h"

// library includes


using namespace xsm;


xsmCodec::xsmCodec() {
  // allocating memory in advance will prevent the app from the need of
  // memory reallocations

  // allocate memory for escaping. it is double the size of the max buffer
  // because if every byte should be escaped in the payload it doubles the size
  mEscapeHelperBuffer = std::vector<uint8_t>(2 * MAX_PAYLOAD_SIZE);
  // allocate memory for incoming packet that is processed from the inut buffer
  mPotentialPayload = std::vector<uint8_t>(MAX_PAYLOAD_SIZE);
}

size_t xsmCodec::encode(const std::vector<uint8_t>& unescapedPayload, PacketBuffer& encodedPacket) {
  // perform escaping in member buffer. this might makes the payload bigger
  escape(unescapedPayload, mEscapeHelperBuffer);
  // now mEscapeHelperBuffer holds the escaped payload
  // assemble the packet and return its size.
  // if the escaped buffer has got too large, return 0 as failure
  if (mEscapeHelperBuffer.size() < MAX_PACKET_SIZE)
    return assemble(mEscapeHelperBuffer, encodedPacket);
  else
    return 0;
}

// The ProtocolConfgi::decode function uses a naive approach, that is,
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

            // note that memory is preallocated in the constructor
            mPotentialPayload.clear();               // this won't free the preallocated mem
            mPotentialPayload.resize(payloadLength); // this neither

            // copy payload to mPotentialPayload buffer
            encodedPackets.get(i + HEADER_SIZE, mPotentialPayload.data(), payloadLength);

            // if there is unescaped delimiter in the payload that might be the start of a new packet
            // discard everything before it
            int delimiterIndex = unescapedDelimiterPos(mPotentialPayload);
            if (delimiterIndex > 0) {
              // discard all before this index
              bytesProcessed = i + HEADER_SIZE + delimiterIndex;
            } else {
              // check payload crc
              uint8_t payloadCrc = 0;
              encodedPackets.get(i + HEADER_SIZE + payloadLength, payloadCrc);
              if (crc8(mPotentialPayload.data(), mPotentialPayload.size()) == payloadCrc) {
                // remove escape characters
                unescape(mPotentialPayload);
                // add it to the output array of payloads
                PacketBuffer packet;
                std::copy(mPotentialPayload.begin(), mPotentialPayload.end(), packet.begin());
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
uint8_t xsmCodec::crc8(const uint8_t* buffer, size_t size) {
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


void xsmCodec::escape(const std::vector<uint8_t>& payload, std::vector<uint8_t>& escapedPayload) {
  // clear output buffer whatever it contained
  escapedPayload.clear(); // this will not free memory in the buffer
  // iterate through the input payload buffer and insert escaped payload in the
  // escapedPayload output buffer
  for (auto it = payload.begin(); it != payload.end(); it++) {
    uint8_t b = *it;
    // don't escape the frame delimiter at first position
    if (it != payload.begin()) {
      // the frame delimiter and the escape byte itself are to be escaped
      if (b == FRAME_DELIMITER || b == ESCAPE_BYTE) {
        escapedPayload.push_back(ESCAPE_BYTE);
      }
    }
    escapedPayload.push_back(b);
  }
}


size_t xsmCodec::assemble(const std::vector<uint8_t>& escapedPayload, PacketBuffer& encodedPacket) {
  // if the escaped data provided is too large, it won't fit in a max size packet
  // this is checked outside as well, but this being a public static method
  // it is better to make sure noone calls it with a wrong parameter
  if (escapedPayload.size() > MAX_PAYLOAD_SIZE)
    return 0;

  // assemble the header
  size_t headerIndex = 0;
  // add frame delimiter first
  encodedPacket[headerIndex++] = FRAME_DELIMITER;
  // add payload length
  encodedPacket[headerIndex++] = static_cast<uint8_t>(escapedPayload.size());
  // add header crc
  encodedPacket[headerIndex++] = crc8(encodedPacket.data(), 2);

  // copy header and escaped payload the packet buffer
  size_t payloadIndex = 0;
  for (; payloadIndex < escapedPayload.size(); payloadIndex++) {
    encodedPacket[headerIndex + payloadIndex] = escapedPayload[payloadIndex];
  }

  // post incremented indexes become sizes

  // add footer: payload crc
  encodedPacket[headerIndex + payloadIndex] = crc8(encodedPacket.data() + HEADER_SIZE, payloadIndex);


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


size_t xsmCodec::unescape(std::vector<uint8_t>& escapedPayload) {
  int skippedBytes = 0;
  // helper variable to remove the first escape character only.
  // this is the case when the escape charcter is escaped in the payload
  bool escaped = false;

  // iterate through escapedPayload buffer and remove escaped characters
  for (auto it = escapedPayload.begin(); it != escapedPayload.end(); it++) {
    if (*it == ESCAPE_BYTE && !escaped) {
      escaped = true;
      // remove escape char inplace
      it = escapedPayload.erase(it);
      // iterator returned by erase will point after the removed element
      if (it != escapedPayload.begin())
        it--;
      ++skippedBytes;
    } else {
      escaped = false;
    }
  }

  return skippedBytes;
}

int xsmCodec::unescapedDelimiterPos(const std::vector<uint8_t>& buffer) {
  uint8_t prevByte = 0;
  // iterate through the buffer
  for (size_t i = 0; i < buffer.size(); i++) {
    if (buffer[i] == FRAME_DELIMITER && prevByte != ESCAPE_BYTE) {
      // index found, return it
      return i;
    }
    prevByte = buffer[i];
  }

  return -1;
}
