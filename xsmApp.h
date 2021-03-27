#ifndef XSM_APP
#define XSM_APP
#include <vector>

#include "xsmDecoder.h"


namespace xsm

{
class xsmApp {
public:
  xsmApp();

  void onMessageReceived(PayloadBuffer payload);
};

} // namespace xsm

#endif