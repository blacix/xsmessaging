#include "xsmApp.h"

#include <iostream>

#include "xsmEndpoint.h"

using namespace xsm;

xsmApp::xsmApp() {
  xsmEndpoint ep(std::bind(&xsmApp::onMessageReceived, this, std::placeholders::_1));
  PacketBuffer packet;
  size_t packetSize = ep.createPacket({'a', 'b', 'c'}, packet);
  ep.receive(packet.data(), packetSize);
}

void xsmApp::onMessageReceived(std::vector<PacketBuffer> packets) {
  std::cout << "packet received: " << packets.size() << std::endl;
}