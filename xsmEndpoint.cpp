#include "xsmEndpoint.h"

#include <iostream>
using namespace xsm;

Endpoint::Endpoint(std::function<void(Message)> callback) : mCallback(callback) {}

void Endpoint::receive(const uint8_t byte) {
  mBufferIn.push(byte);
  process();
}

void Endpoint::receive(const uint8_t* bytes, size_t size) {
  for (size_t i = 0; i < size; ++i) {
    receive(bytes[i]);
  }
}

void Endpoint::receive(const std::vector<uint8_t>& bytes) {
  for (auto it = bytes.begin(); it != bytes.end(); ++it) {
    receive(*it);
  }
}

void Endpoint::process() {
  size_t byteProcessed = mProtocolDecoder.decode(mBufferIn, mReceivedMessages);

  // remove the bytes from the buffer that are already processed
  if (byteProcessed > 0) {
    std::cout << "bytes processed: " << byteProcessed << std::endl;
    mBufferIn.pop(byteProcessed);
    if (!mReceivedMessages.empty()) {
      for (const Message& message : mReceivedMessages) {
        mCallback(message);
      }
      mReceivedMessages.clear();
    }
  }
}

