#ifndef XSM_APP_ENDPOINT_H
#define XSM_APP_ENDPOINT_H
#include "xsmEndpoint.h"
class xsmAppEndpoint : public xsm::Endpoint {
public:
  xsmAppEndpoint(std::function<void(xsm::Message)> callback);
  virtual void send(const xsm::Message& message);
};
#endif // !XSM_APP_ENDPOINT_H
