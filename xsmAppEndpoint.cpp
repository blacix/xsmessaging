#include "xsmAppEndpoint.h"

xsmAppEndpoint::xsmAppEndpoint(std::function<void(xsm::Message)> callback) : xsm::Endpoint(callback){

}

void xsmAppEndpoint::send(const xsm::Message& message) {
  xsm::Frame frame = mProtocolCoder.encode(message);
  receive(frame.getData().data(), frame.getSize());
}
