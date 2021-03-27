#include "xsmDecoder.h"
#include "CRCMaximDallas.h"
#include "xsmUtils.h"

using namespace xsm;

// The ProtocolConfig::decode function uses a naive approach, that is,
// it always starts interpreting the input buffer from the beginning.This has, of course, impact on the performance.
size_t xsmDecoder::decode(const RingBuffer& encodedPackets, std::vector<PacketBuffer>& decodedPackets) {
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
        if (crc8MaximDallas(mHeader.data(), HEADER_SIZE - 1) == mHeader[2]) {
          // check if we have enough data based on the payload length
          uint16_t packetLength = HEADER_SIZE + payloadLength + FOOTER_SIZE;
          if (encodedPackets.capacity() >= i + packetLength) {
            // extract payload

            // copy payload to mPotentialPayload buffer
            encodedPackets.get(i + HEADER_SIZE, mPotentialPayload.data(), payloadLength);

            // if there is unescaped delimiter in the payload that might be the start of a new packet
            // discard everything before it
            int delimiterIndex = xsmUtils::unescapedDelimiterPos(mPotentialPayload, payloadLength);
            if (delimiterIndex > 0) {
              // discard all before this index
              bytesProcessed = i + HEADER_SIZE + delimiterIndex;
            } else {
              // check payload crc
              uint8_t payloadCrc = 0;
              encodedPackets.get(i + HEADER_SIZE + payloadLength, payloadCrc);
              if (crc8MaximDallas(mPotentialPayload.data(), payloadLength) == payloadCrc) {
                // remove escape characters
                xsmUtils::unescape(mPotentialPayload, payloadLength, mEscapeHelperBuffer);
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


