#include "xsmApp.h"

#include <iostream>

#include "xsmEndpoint.h"

using namespace xsm;

xsmApp::xsmApp() {
  xsmEndpoint ep(std::bind(&xsmApp::onMessageReceived, this, std::placeholders::_1));

  uint8_t data[256];

  PacketBuffer packet;
  size_t packetSize = ep.createPacket({'a', 'b', 'c'}, packet);
  std::copy(packet.begin(), packet.end(), data);

  packetSize = ep.createPacket({'d', 'e', 'f'}, packet);
  std::copy(packet.begin(), packet.end(), data + packetSize);

  for (size_t i = 0; i < 14; ++i) {
    ep.receive(data[i]);
  }
}

void xsmApp::onMessageReceived(PayloadBuffer payload) {
  std::cout << "packet received" << std::endl;
}