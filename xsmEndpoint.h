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
  virtual void send(const Message& message);
  void receive(const uint8_t byte);

protected:
  virtual void sendSpecific(const uint8_t* data, const size_t size) = 0;
  
  Decoder mDecoder;
  Coder mEncoder;

private:
  RingBuffer mBufferIn;
  std::function<void(Message)> mCallback;
  
};

} // namespace xsm
#endif