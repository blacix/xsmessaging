#ifndef XSM_ENDPOINT
#define XSM_ENDPOINT
#include <functional>

#include "xsmCodec.h"
#include "xsmRingBuffer.h"
namespace xsm {

class xsmEndpoint {
public:
  xsmEndpoint(std::function<void(std::vector<PacketBuffer>)>);
  void receive(const uint8_t byte);
  void receive(const uint8_t* bytes, size_t size);
  void receive(const std::vector<uint8_t>& bytes);

  const size_t createPacket(const std::vector<uint8_t>&& payload, PacketBuffer& packet);

private:
  void process();

  std::function<void(std::vector<PacketBuffer>)> mCallback;
  RingBuffer mBufferIn;
  xsmCodec mProtocolCodec;
  std::vector<PacketBuffer> mReceivedPackets;
};

} // namespace xsm
#endif