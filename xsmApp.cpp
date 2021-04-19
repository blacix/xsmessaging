#include "xsmApp.h"

#include <iostream>

#include "xsmAppEndpoint.h"
#include "xsmAppUtils.h"


xsmApp::xsmApp() {
  xsmAppEndpoint endpoint(*this);

  xsm::Message msg1{{'a', 'b', 'c'}, 3};
  xsm::Message msg2{{'c', 'd', 'e'}, 3};
  endpoint.sendMessage(msg1);
  endpoint.sendMessage(msg2);
}

void xsmApp::onMessageReceived(const xsm::Message message) {
  std::cout << "message received" << std::endl;
  xsm::AppUtils::print(message.Data.data(), message.Size);
}
