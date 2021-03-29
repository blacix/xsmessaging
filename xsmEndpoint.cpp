#include "xsmEndpoint.h"

#include <iostream>
using namespace xsm;

Endpoint::Endpoint(std::function<void(Message)> callback) : mDecoder(callback), mCallback(callback) {}

void Endpoint::send(const Message& message) {
  Frame frame = mEncoder.encode(message);
  sendSpecific(frame.getData().data(), frame.getSize());
}

 void Endpoint::receive(const uint8_t byte) {
  mDecoder.receive(byte);
}
