#ifndef _CRC_MAXIM_DALLAS
#define _CRC_MAXIM_DALLAS

#include <cstdint>

namespace xsm{
  // CRC for Maxim/Dallas 1-Wire
  uint8_t crc8MaximDallas(const uint8_t* buffer, const size_t size) {
    uint8_t crc = 0;
    uint8_t sum = 0;
    for (size_t i = 0; i < size; i++) {
      uint8_t byte = buffer[i];
      for (uint8_t tmp = 8; tmp; tmp--) {
        sum = (crc ^ byte) & 0x01;
        crc >>= 1;
        if (sum)
          crc ^= 0x8C;
        byte >>= 1;
      }
    }
    return crc;
  }
}

#endif