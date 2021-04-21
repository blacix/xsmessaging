#ifndef XSM_APP
#define XSM_APP
#include <vector>

#include "xsmReceiver.h"
#include "xsmSender.h"


class xsmApp : public xsm::ISender, xsm::IReceiver {
public:
  xsmApp();

  void send(const uint8_t* data, const size_t size) override;
  void onMessageReceived(const xsm::Message& message) override;

private:
  xsm::Sender mSender;
  xsm::Receiver mReceiver;
};

#endif
