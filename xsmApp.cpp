#include "xsmApp.h"

#include <iostream>

#include "xsmEndpoint.h"
#include "xsmAppUtils.h"


xsmApp::xsmApp() {
  xsm::Endpoint ep(std::bind(&xsmApp::onMessageReceived, this, std::placeholders::_1));

  xsm::Payload payload{{'a', 'b', 'c'}, 3};
  ep.send(payload);
}

void xsmApp::onMessageReceived(xsm::Payload payload) {
  std::cout << "packet received" << std::endl;
  xsm::AppUtils::print(payload.Data.data(), payload.DataSize);
}