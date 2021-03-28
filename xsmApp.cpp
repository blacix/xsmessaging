#include "xsmApp.h"

#include <iostream>

#include "xsmAppEndpoint.h"
#include "xsmAppUtils.h"


xsmApp::xsmApp() {
  xsmAppEndpoint endpoint(std::bind(&xsmApp::onMessageReceived, this, std::placeholders::_1));

  xsm::Message msg1{{'a', 'b', 'c'}, 3};
  xsm::Message msg2{{'c', 'd', 'e'}, 3};
  endpoint.send(msg1);
  endpoint.send(msg2);
}

void xsmApp::onMessageReceived(xsm::Message message) {
  std::cout << "message received" << std::endl;
  xsm::AppUtils::print(message.Data.data(), message.Size);
}