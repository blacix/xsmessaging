#ifndef IRECEIVER_H
#define IRECEIVER_H

#include <cstdint>

#include "xsmTypes.h"

namespace xsm {
class IReceiver {
public:
  virtual void onMessageReceived(const Message& message) = 0;
};
} // namespace xsm

#endif
