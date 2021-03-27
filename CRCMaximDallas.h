#ifndef _CRC_MAXIM_DALLAS
#define _CRC_MAXIM_DALLAS

#include <cstdint>

namespace xsm {
// CRC for Maxim/Dallas 1-Wire
uint8_t crc8MaximDallas(const uint8_t* buffer, const size_t size);
} // namespace xsm

#endif