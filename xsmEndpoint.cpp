#include "xsmEndpoint.h"
#include "xsmUtils.h"
#include <iostream>
using namespace xsm;

Endpoint::Endpoint(IMessageCallback& callback) : mCallback(callback), mDecoder(*this) {}

void Endpoint::sendMessage(const Message& message) {
  Utils::escape(message, mEscapedPayload);
  if (mEscapedPayload.Size > 0) {
    Frame frame{message};
    sendSpecific(frame.getData().data(), frame.getSize());
  }
}

void Endpoint::receive(const uint8_t byte) {
  mDecoder.receive(byte);
}

void Endpoint::onFrameReceived(const Frame& frame) {
  Message message;
  message.Size = Utils::unescape(frame.getPayloadBuffer(), frame.getPayloadSize(), message.Data);
  mCallback.onMessageReceived(message);
}