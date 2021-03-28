#ifndef XSM_ENDPOINT
#define XSM_ENDPOINT
#include <functional>
#include <vector>

#include "xsmCoder.h"
#include "xsmDecoder.h"


namespace xsm {

class Endpoint {
public:
  Endpoint(std::function<void(PayloadBuffer)>);
  void receive(const uint8_t byte);
  void receive(const uint8_t* bytes, size_t size);
  void receive(const std::vector<uint8_t>& bytes);

  Packet createPacket(const std::vector<uint8_t>&& data);

private:
  void process();

  std::function<void(PayloadBuffer)> mCallback;
  RingBuffer mBufferIn;
  Decoder mProtocolCodec;
  Coder mProtocolCoder;
  std::vector<PayloadBuffer> mReceivedPackets;
};

} // namespace xsm
#endif