#ifndef XSM_ENDPOINT
#define XSM_ENDPOINT
#include <functional>
#include <vector>

#include "xsmCoder.h"
#include "xsmDecoder.h"
#include "xsmFrame.h"

namespace xsm {

class Endpoint : public IFrameCallback{
public:
  Endpoint(IMessageCallback& callback);
  virtual ~Endpoint() = default;
  virtual void sendMessage(const Message& message);
  void receive(const uint8_t byte);
  void onFrameReceived(const Frame& frame) override;

protected:
  virtual void sendSpecific(const uint8_t* data, const size_t size) = 0;
  IMessageCallback& mCallback;
  Decoder mDecoder;
  Coder mEncoder;

private:
  
};

} // namespace xsm

#endif
