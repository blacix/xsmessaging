#include "xsmSender.h"

#include <iostream>

#include "xsmUtils.h"

using namespace xsm;

Sender::Sender(ISender& callback) : mSender(callback) {}

void Sender::sendMessage(const Message& message) {
  Utils::escape(message, mEscapedPayload);
  if (mEscapedPayload.Size > 0) {
    Frame frame{mEscapedPayload};
    mSender.send(frame.getData().data(), frame.getSize());
  }
}
