#ifndef XSM_DEFINES_H
#define XSM_DEFINES_H

namespace xsm {

// packet delimiter byte
// 0xab = 802.3 Start Frame Delimiter
static constexpr uint8_t FRAME_DELIMITER = 0b10101011;
static constexpr uint8_t FRAME_DELIMITER2 = ~FRAME_DELIMITER;
// escape byte
// 0x2f = '/' ascii char
static const uint8_t ESCAPE_BYTE = 0b00101111;
// payload length position in header
static constexpr uint8_t PAYLOAD_SIZE_INDEX = 2;
static constexpr uint8_t HEADER_CRC_INDEX = 3;
static constexpr uint8_t HEADER_SIZE = 4;
static constexpr uint8_t FOOTER_SIZE = 1;
static constexpr uint8_t MIN_PAYLOAD_SIZE = 1;
static constexpr uint8_t MAX_PAYLOAD_SIZE = 128;
static constexpr uint16_t MAX_FRAME_SIZE = HEADER_SIZE + MAX_PAYLOAD_SIZE + FOOTER_SIZE;
static constexpr uint16_t MIN_FRAME_SIZE = HEADER_SIZE + MIN_PAYLOAD_SIZE + FOOTER_SIZE;
static constexpr size_t INPUT_BUFFER_SIZE = 2048;

} // namespace xsm

#endif
