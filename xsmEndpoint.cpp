#include "xsmEndpoint.h"
#include "xsmUtils.h"
#include <iostream>
using namespace xsm;

Endpoint::Endpoint(IMessageCallback& callback) : mCallback(callback), mDecoder(*this) {}

void Endpoint::sendMessage(const Message& message) {
  Frame frame = mEncoder.encode(message);
  sendSpecific(frame.getData().data(), frame.getSize());
}

void Endpoint::receive(const uint8_t byte) {
  mDecoder.receive(byte);
}

void Endpoint::onFrameReceived(const Frame& frame) {
  Message message;
  Utils::unescape(frame.getPayload(), message);
  mCallback.onMessageReceived(message);
}