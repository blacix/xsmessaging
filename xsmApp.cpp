#include "xsmApp.h"
#include "xsmEndpoint.h"
#include <iostream>

using namespace xsm;

xsmApp::xsmApp()
{
	xsmEndpoint ep(std::bind(&xsmApp::onMessageReceived, this, std::placeholders::_1));
	PacketBuffer packet;
	size_t packetSize = ep.createPacket({ 'a', 'b', 'c' }, packet);
	ep.receive(packet.data(), packetSize);
}

void xsmApp::onMessageReceived(std::vector<PacketBuffer> packets)
{
	for (PacketBuffer packet : packets)
	{
		std::cout << "packet received: " << std::endl;
	}
	
}