#ifndef ISENDER_H
#define ISENDER_H
#define ISENDER_H
#include <cstdint>

namespace xsm {
class ISender {
public:
  virtual void send(const uint8_t* data, const size_t size) = 0;

private:
};
} // namespace xsm

#endif
