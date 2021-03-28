#include "xsmUtils.h"

using namespace xsm;

void Utils::escape(const Message& unescapedPayload, Message& escapedPayload) {

  size_t escapedPayloadSize = 0;
  // iterate through the input payload buffer and insert escaped payload in the
  // escapedPayload output buffer
  for (size_t i = 0; i < unescapedPayload.Size && escapedPayloadSize < escapedPayload.Data.size(); i++) {
    uint8_t b = unescapedPayload.Data[i];
    // the frame delimiter and the escape byte itself are to be escaped
    if (b == FRAME_DELIMITER || b == ESCAPE_BYTE) {
      escapedPayload.Data[escapedPayloadSize] = ESCAPE_BYTE;
      ++escapedPayloadSize;
    }

    if (escapedPayloadSize < escapedPayload.Data.size()) {
      escapedPayload.Data[escapedPayloadSize] = b;
      ++escapedPayloadSize;
    }
  }
  escapedPayload.Size = escapedPayloadSize;
}


void Utils::unescape(const Message& escapedPayload, Message& unEscapedPayload) {
  int skippedBytes = 0;
  // helper variable to remove the first escape character only.
  // this is the case when the escape charcter is escaped in the payload
  bool escaped = false;

  // iterate through escapedPayload buffer and remove escaped characters
  size_t unescapedPayloadSize = 0;
  for (size_t i = 0; i < escapedPayload.Size; i++) {
    if (escapedPayload.Data[i] == ESCAPE_BYTE) {
      if (escaped) {
        // prev byte was the escape character
        // this byte is also the escape character
        escaped = false;
        // skip this byte
      } else {
        // prev byte was not the escape character
        escaped = true;
        unEscapedPayload.Data[unescapedPayloadSize] = escapedPayload.Data[i];
        ++unescapedPayloadSize;
      }
    } else {
      unEscapedPayload.Data[unescapedPayloadSize] = escapedPayload.Data[i];
      ++unescapedPayloadSize;
    }    
  }

  unEscapedPayload.Size = unescapedPayloadSize;
}

int Utils::unescapedDelimiterPos(const MessageBuffer& buffer, const size_t bufferSize) {
  uint8_t prevByte = 0;
  // iterate through the buffer
  for (size_t i = 0; i < bufferSize; i++) {
    if (buffer[i] == FRAME_DELIMITER && prevByte != ESCAPE_BYTE) {
      // index found, return it
      return static_cast<int>(i);
    }
    prevByte = buffer[i];
  }

  return -1;
}