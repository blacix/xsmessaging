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
  Endpoint(std::function<void(Message)>);
  void send(const Message& message);
  
  void receive(const uint8_t byte);
  void receive(const uint8_t* bytes, size_t size);
  void receive(const std::vector<uint8_t>& bytes);

private:
  void process();

  std::function<void(Message)> mCallback;
  RingBuffer mBufferIn;
  Decoder mProtocolDecoder;
  Coder mProtocolCoder;
  std::vector<Message> mReceivedPayloads;
};

} // namespace xsm
#endif