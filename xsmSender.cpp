#include "xsmSender.h"

#include <iostream>

#include "xsmUtils.h"

using namespace xsm;

Sender::Sender(const Escaping escaping, ISender& sendImpl) : mEscaping(escaping), mSendImpl(sendImpl) {}

void Sender::send(const Message& message) {
  if (mEscaping == Escaping::ON) {
    bool success = Utils::escape(message, mEscapedPayload);
    if (success) {
      Frame frame{mEscapedPayload};
      mSendImpl.send(frame.getFrameBuffer().data(), frame.getSize());
    }
  } else {
    Frame frame{message};
    mSendImpl.send(frame.getFrameBuffer().data(), frame.getSize());
  }
}
