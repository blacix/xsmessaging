#ifndef XSM_APP
#define XSM_APP
#include <vector>

#include "xsmDecoder.h"

class xsmApp {
public:
  xsmApp();

  void onMessageReceived(xsm::PayloadBuffer payload);
};


#endif