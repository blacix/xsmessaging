#include "xsmEndpoint.h"

#include <iostream>
using namespace xsm;

Endpoint::Endpoint(std::function<void(Payload)> callback) : mCallback(callback) {}

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
  size_t byteProcessed = mProtocolCodec.decode(mBufferIn, mReceivedPackets);

  // remove the bytes from the buffer that are already processed
  if (byteProcessed > 0) {
    std::cout << "bytes processed: " << byteProcessed << std::endl;
    mBufferIn.pop(byteProcessed);
    if (!mReceivedPackets.empty()) {
      for (Payload packet : mReceivedPackets) {
        mCallback(packet);
      }
      mReceivedPackets.clear();
    }
  }
}

Packet Endpoint::createPacket(const std::vector<uint8_t>&& data) {
  Packet packet;
  Payload payload;
  payload.DataSize = data.size();
  std::copy(data.begin(), data.end(), payload.Data.begin());

  mProtocolCoder.encode(payload, packet);
  return packet;
}
