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

void Frame::setEscapedPayloadBuffer(const MessageBuffer& payload) {
  std::copy(payload.begin(), payload.end(), mData.begin() + HEADER_SIZE);
}

void Frame::setEscapedPayload(const Message& payload) {
  setEscapedPayloadBuffer(payload.Data);
  setPayloadSize(static_cast<uint8_t>(payload.Size));
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

MessageBuffer Frame::getPayloadBuffer() const {
  MessageBuffer payloadBuffer;
  copyMessage(payloadBuffer, getPayloadSize());
  return payloadBuffer;
}

Message Frame::getPayload() const {
  Message payload;
  uint8_t payloadSize = getPayloadSize();
  copyMessage(payload.Data, payloadSize);
  payload.Size = payloadSize;
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

void Frame::copyMessage(MessageBuffer& message, const uint8_t payloadSize) const {
  std::copy(mData.begin() + HEADER_SIZE, mData.begin() + payloadSize, message.begin());
}