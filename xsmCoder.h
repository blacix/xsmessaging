#ifndef XSM_CODER_H
#define XSM_CODER_H
#include "xsmFrame.h"
#include "xsmTypes.h"

namespace xsm {

class Coder {

public:
  const Frame encode(const Message& payload);

private:
  // preallocated helper buffer for escaping
  Message mEscapedPayload;
};

} // namespace xsm

#endif // !XSM_CODER_H