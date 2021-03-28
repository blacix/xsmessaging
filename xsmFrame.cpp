#include "xsmFrame.h"
using namespace xsm;

Frame::Frame() {
  mData[0] = FRAME_DELIMITER;
  setPayloadSize(0);
}

void Frame::setPayloadSize(const uint8_t size) {
  mData[PAYLOAD_SIZE_INDEX] = size;
}

void Frame::setHeaderCrc(const uint8_t crc) {
  mData[HEADER_CRC_INDEX] = crc;
}

void Frame::setEscapedPayload(const PayloadBuffer& payload) {
  std::copy(payload.begin(), payload.end(), mData.begin() + HEADER_SIZE);
}

void Frame::setPayloadCrc(const uint8_t crc) {
  mData[static_cast<size_t>(HEADER_SIZE) + static_cast<size_t>(getPayloadSize())] = crc;
}

uint8_t Frame::getPayloadSize() const {
  return mData[PAYLOAD_SIZE_INDEX];
}

uint8_t Frame::getHeaderCrc() const {
  return mData[HEADER_CRC_INDEX];
}

PayloadBuffer Frame::getPayload() const {
  PayloadBuffer payload;
  std::copy(mData.begin() + HEADER_SIZE, mData.begin() + getPayloadSize(), payload.begin());
  return payload;
}

const PacketBuffer& Frame::getData() const {
  return mData;
}

uint8_t Frame::getPayloadCrc() const {
  return mData[static_cast<size_t>(HEADER_SIZE) + static_cast<size_t>(getPayloadSize())];
}

uint8_t Frame::getSize() const {
  return HEADER_SIZE + getPayloadSize() + FOOTER_SIZE;
}