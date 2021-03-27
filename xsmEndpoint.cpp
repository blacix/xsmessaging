#include "xsmEndpoint.h"

#include <iostream>
using namespace xsm;

xsmEndpoint::xsmEndpoint(std::function<void(std::vector<PacketBuffer>)> callback) : mCallback(callback) {}

void xsmEndpoint::receive(const uint8_t byte) {
  mBufferIn.push(byte);
  process();
}

void xsmEndpoint::receive(const uint8_t* bytes, size_t size) {
  for (size_t i = 0; i < size; ++i) {
    receive(bytes[i]);
  }
}

void xsmEndpoint::receive(const std::vector<uint8_t>& bytes) {
  for (auto it = bytes.begin(); it != bytes.end(); ++it) {
    receive(*it);
  }
}

void xsmEndpoint::process() {
  size_t byteProcessed = mProtocolCodec.decode(mBufferIn, mReceivedPackets);

  // remove the bytes from the buffer that are already processed
  if (byteProcessed > 0) {
    std::cout << "bytes processed: " << byteProcessed << std::endl;
    mBufferIn.pop(byteProcessed);
    if (!mReceivedPackets.empty()) {
      mCallback(mReceivedPackets);
      mReceivedPackets.clear();
    }
  }
}

const size_t xsmEndpoint::createPacket(const std::vector<uint8_t>&& data, PacketBuffer& packet) {
  PayloadBuffer payloadBuffer;
  std::copy(data.begin(), data.end(), payloadBuffer.begin());
  return mProtocolCoder.encode(payloadBuffer, data.size(), packet);
}
