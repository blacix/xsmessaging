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
  Endpoint(std::function<void(Message)> callback);
  virtual ~Endpoint() = default;
  virtual void send(const Message& message) = 0;

  void receive(const uint8_t byte);
  //void receive(const uint8_t* bytes, size_t size);
  //void receive(const std::vector<uint8_t>& bytes);

protected:
  Coder mProtocolCoder;

private:
  //void process();

  RingBuffer mBufferIn;
  Decoder mProtocolDecoder;

  std::vector<Message> mReceivedMessages;

  std::function<void(Message)> mCallback;
  
};

} // namespace xsm
#endif