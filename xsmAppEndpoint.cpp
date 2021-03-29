#include "xsmAppEndpoint.h"

xsmAppEndpoint::xsmAppEndpoint(std::function<void(xsm::Message)> callback) : xsm::Endpoint(callback){

}

void xsmAppEndpoint::sendSpecific(const uint8_t* data, const size_t size) {
  for (size_t i = 0; i < size; ++i) {
    receive(data[i]);
  }
}
