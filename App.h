#ifndef XSM_APP
#define XSM_APP
#include <vector>

#include "xsmReceiver.h"
#include "xsmSender.h"

class Protocol : public xsm::IReceiver {
public:
  Protocol(xsm::Sender& sender);
  void onMessageReceived(const xsm::Message& message) override;

private:
  xsm::Sender mSender;
};

class Hook : public xsm::ISender {
public:
  Hook();
  void send(const uint8_t* data, const size_t size);
  void setReceiver(xsm::Receiver* receiver);

private:
  xsm::Receiver* mReceiver;
};

class xsmApp : xsm::IReceiver {
public:
  xsmApp();

private:
};

#endif
