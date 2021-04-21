#ifndef _XSM_CRC_H
#define _XSM_CRC_H

#include <cstddef>
#include <cstdint>

namespace xsm {
// CRC for Maxim/Dallas 1-Wire
uint8_t crc8(const uint8_t* buffer, const size_t size);
} // namespace xsm

#endif
