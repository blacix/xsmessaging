#include "App.h"

#include <iostream>

#include "AppUtils.h"

xsmApp::xsmApp() : mSender(xsm::Escaping::OFF, *this), mReceiver(xsm::Escaping::OFF, * this) {
  xsm::Message msg1{{'a', 'b', 'c', 'd'}, 4};
  msg1.Data[3] = xsm::FRAME_DELIMITER; // test escaping
  mSender.send(msg1);

  xsm::Message msg2{{'a', 'b', 'c', 'd'}, 4};
  msg2.Data[3] = xsm::ESCAPE_BYTE; // test escaping
  mSender.send(msg2);
}

void xsmApp::send(const uint8_t* data, const size_t size) {
  for (size_t i = 0; i < size; ++i) {
    mReceiver.receive(data[i]);
  }
}

void xsmApp::onMessageReceived(const xsm::Message& message) {
  std::cout << "message received" << std::endl;
  AppUtils::print(message.Data.data(), message.Size);
}
