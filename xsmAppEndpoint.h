#ifndef XSM_APP_ENDPOINT_H
#define XSM_APP_ENDPOINT_H
#include "xsmEndpoint.h"

class xsmAppEndpoint : public xsm::Endpoint {
public:
  xsmAppEndpoint(xsm::IMessageCallback& callback);

protected:
  virtual void sendSpecific(const uint8_t* data, const size_t size) override;
};
#endif // !XSM_APP_ENDPOINT_H
