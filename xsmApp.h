#ifndef XSM_APP
#define XSM_APP
#include <functional>
#include <vector>
#include "xsmDecoder.h"
namespace xsm
{
class xsmApp
{
public:
	xsmApp();

	void onMessageReceived(std::vector<PacketBuffer> messages);
};
}


#endif