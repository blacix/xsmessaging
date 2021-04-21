#ifndef XSM_UTILS_H
#define XSM_UTILS_H

#include "xsmFrame.h"
#include "xsmTypes.h"

namespace xsm {

class Utils {
public:
  // Function to check for characters in the payload that are to be escaped.
  // These are the Frame Delimiter and the escape character itself.
  static bool escape(const Message& unescapedPayload, Message& escapedPayload);

  // Helper function to remove escape characters from an escaped payload
  // returns the number of escae bytes removed from the buffer
  static size_t unescape(const MessageBuffer& escapedPayload,
                         const size_t escapedPayloadSize,
                         MessageBuffer& unEscapedPayload);

  // helper method that searches for unescaped delimiter in the buffer
  static int unescapedDelimiterPos(const MessageBuffer& buffer, const size_t bufferSize);
};
} // namespace xsm
#endif
