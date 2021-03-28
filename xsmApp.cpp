#include "xsmApp.h"

#include <iostream>

#include "xsmEndpoint.h"
#include "xsmAppUtils.h"


xsmApp::xsmApp() {
  xsm::Endpoint ep(std::bind(&xsmApp::onMessageReceived, this, std::placeholders::_1));

  xsm::Message msg{{'a', 'b', 'c'}, 3};
  ep.send(msg);
}

void xsmApp::onMessageReceived(xsm::Message payload) {
  std::cout << "packet received" << std::endl;
  xsm::AppUtils::print(payload.Data.data(), payload.Size);
}