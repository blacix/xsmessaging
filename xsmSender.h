#ifndef XSM_SENDER
#define XSM_SENDER
#include <functional>
#include <vector>

#include "xsmFrame.h"
#include "xsmISender.h"

namespace xsm {

class Sender {
public:
  Sender(const Escaping escaping, ISender& sendImpl);
  virtual ~Sender() = default;
  virtual void send(const Message& message);

private:
  Escaping mEscaping;
  ISender& mSendImpl;
  Message mEscapedPayload;
};

} // namespace xsm

#endif
