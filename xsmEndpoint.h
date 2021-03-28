#ifndef XSM_ENDPOINT
#define XSM_ENDPOINT
#include <functional>
#include <vector>

#include "xsmCoder.h"
#include "xsmDecoder.h"
#include "xsmFrame.h"

namespace xsm {

class Endpoint {
public:
  Endpoint(std::function<void(Payload)>);
  void receive(const uint8_t byte);
  void receive(const uint8_t* bytes, size_t size);
  void receive(const std::vector<uint8_t>& bytes);

  Packet createPacket(const std::vector<uint8_t>&& data);
  Packet createPacket(const Payload& data);

  Frame createFrame(const Payload& data);

private:
  void process();

  std::function<void(Payload)> mCallback;
  RingBuffer mBufferIn;
  Decoder mProtocolDecoder;
  Coder mProtocolCoder;
  std::vector<Payload> mReceivedPayloads;
};

} // namespace xsm
#endif