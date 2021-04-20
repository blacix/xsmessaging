#include "xsmApp.h"

#include <iostream>

#include "xsmAppEndpoint.h"
#include "xsmAppUtils.h"
#include "xsmDefines.h"

xsmApp::xsmApp() {
  xsmAppEndpoint endpoint(*this);
  xsm::Message msg1{{'a', 'b', 'c', 'd'}, 4};
  msg1.Data[3] = xsm::FRAME_DELIMITER; // test escaping
  endpoint.sendMessage(msg1);

  xsm::Message msg2{{'a', 'b', 'c', 'd'}, 4};
  msg2.Data[3] = xsm::ESCAPE_BYTE; // test escaping
  endpoint.sendMessage(msg2);
}

void xsmApp::onMessageReceived(const xsm::Message message) {
  std::cout << "message received" << std::endl;
  xsm::AppUtils::print(message.Data.data(), message.Size);
}
