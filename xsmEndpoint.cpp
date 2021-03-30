#include "xsmEndpoint.h"

#include <iostream>
using namespace xsm;

Endpoint::Endpoint(MessageCallback callback) : mDecoder(callback) {}

void Endpoint::sendMessage(const Message& message) {
  Frame frame = mEncoder.encode(message);
  sendSpecific(frame.getData().data(), frame.getSize());
}

void Endpoint::receive(const uint8_t byte) {
  mDecoder.receive(byte);
}
