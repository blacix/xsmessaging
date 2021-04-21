#ifndef XSM_SENDER
#define XSM_SENDER
#include <functional>
#include <vector>

#include "xsmFrame.h"
#include "xsmISender.h"

namespace xsm {

class Sender {
public:
  Sender(ISender& sender);
  virtual ~Sender() = default;
  virtual void sendMessage(const Message& message);

protected:
  ISender& mSender;

private:
  Message mEscapedPayload;
};

} // namespace xsm

#endif
