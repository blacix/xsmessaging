#include "xsmSender.h"

#include <iostream>

#include "xsmUtils.h"

using namespace xsm;

Sender::Sender(ISender& sendImpl) : mSendImpl(sendImpl) {}

void Sender::send(const Message& message) {
  Utils::escape(message, mEscapedPayload);
  if (mEscapedPayload.Size > 0) {
    Frame frame{mEscapedPayload};
    mSendImpl.send(frame.getData().data(), frame.getSize());
  }
}
