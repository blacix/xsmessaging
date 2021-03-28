#include "xsmApp.h"

#include <iostream>

#include "xsmEndpoint.h"
#include "xsmAppUtils.h"


xsmApp::xsmApp() {
  xsm::Endpoint ep(std::bind(&xsmApp::onMessageReceived, this, std::placeholders::_1));

  uint8_t data[256];

  xsm::Payload p{{'a', 'b', 'c'}, 3};
  xsm::Frame frame = ep.createFrame(p);

  xsm::Packet packet = ep.createPacket({'a', 'b', 'c'}); 
  std::copy(packet.Data.begin(), packet.Data.end(), data);

  packet = ep.createPacket({'d', 'e', 'f'});
  std::copy(packet.Data.begin(), packet.Data.end(), data + packet.DataSize);
  

  for (size_t i = 0; i < 14; ++i) {
    ep.receive(data[i]);
  }
}

void xsmApp::onMessageReceived(xsm::Payload payload) {
  std::cout << "packet received" << std::endl;
  xsm::AppUtils::print(payload.Data.data(), payload.DataSize);
}