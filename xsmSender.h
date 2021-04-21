#ifndef XSM_SENDER
#define XSM_SENDER
#include <functional>
#include <vector>

#include "xsmFrame.h"
#include "xsmISender.h"

namespace xsm {

class Sender {
public:
  Sender(ISender& sendImpl);
  virtual ~Sender() = default;
  virtual void send(const Message& message);

private:
  ISender& mSendImpl;
  Message mEscapedPayload;
};

} // namespace xsm

#endif
