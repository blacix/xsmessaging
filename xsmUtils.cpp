#include "xsmUtils.h"

using namespace xsm;

size_t Utils::escape(const PayloadBuffer& unescapedPayload,
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

size_t Utils::unescape(const PayloadBuffer& escapedPayload,
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

int Utils::unescapedDelimiterPos(const PayloadBuffer& buffer, const size_t bufferSize) {
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