#ifndef XSM_UTILS_H
#define XSM_UTILS_H

#include "xsmTypes.h"

namespace xsm {

class Utils {
public:
  // Function to check for characters in the payload that are to be escaped.
  // These are the Frame Delimiter and the escape character itself.
  static void escape(const Payload& unescapedPayload, Payload& escapedPayload);

  // Helper function to remove escape characters from an escaped payload
  // returns the number of escae bytes removed from the buffer
  static void unescape(const Payload& escapedPayload, Payload& unEscapedPayload);

  // helper method that searches for unescaped delimiter in the buffer
  static int unescapedDelimiterPos(const PayloadBuffer& buffer, const size_t bufferSize);
};
} // namespace xsm
#endif