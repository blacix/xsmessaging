#ifndef XSM_APP
#define XSM_APP
#include <vector>

#include "xsmDecoder.h"
#include "xsmTypes.h"

class xsmApp : public xsm::IMessageCallback {
public:
  xsmApp();

  void onMessageReceived(const xsm::Message message) override;
};

#endif
