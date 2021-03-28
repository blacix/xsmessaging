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
  size_t byteProcessed = mProtocolDecoder.decode(mBufferIn, mReceivedPayloads);

  // remove the bytes from the buffer that are already processed
  if (byteProcessed > 0) {
    std::cout << "bytes processed: " << byteProcessed << std::endl;
    mBufferIn.pop(byteProcessed);
    if (!mReceivedPayloads.empty()) {
      for (Payload packet : mReceivedPayloads) {
        mCallback(packet);
      }
      mReceivedPayloads.clear();
    }
  }
}

Packet Endpoint::createPacket(const std::vector<uint8_t>&& data) {
  Payload payload;
  payload.DataSize = data.size();
  std::copy(data.begin(), data.end(), payload.Data.begin());
  return createPacket(payload);
}

Packet Endpoint::createPacket(const Payload& data) {
  Packet packet;
  mProtocolCoder.encode(data, packet);
  return packet;
}


Frame Endpoint::createFrame(const Payload& payload) {
  Frame frame;
  mProtocolCoder.encode(payload, frame);
  return frame;
}
