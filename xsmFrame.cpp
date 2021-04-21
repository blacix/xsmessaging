#include "xsmFrame.h"

#include "xsmCRC.h"

using namespace xsm;

Frame::Frame() {
  mHeader[0] = FRAME_DELIMITER;
  mHeader[1] = FRAME_DELIMITER2;
  setPayloadSize(0);
  mPayloadCrc = 0;
}

Frame::Frame(const Message& payload) : Frame() {
  setPayloadSize(static_cast<uint8_t>(payload.Size));
  setPayload(payload);
  setHeaderCrc(crc8(mHeader.data(), HEADER_SIZE - 1));
  setPayloadCrc(crc8(payload.Data.data(), payload.Size));
}

void Frame::setPayloadSize(const uint8_t size) {
  mHeader[PAYLOAD_SIZE_INDEX] = size;
}

void Frame::setHeaderCrc(const uint8_t crc) {
  mHeader[HEADER_CRC_INDEX] = crc;
}

void Frame::setPayloadBuffer(const MessageBuffer& payload) {
  std::copy(payload.begin(), payload.end(), mPayload.begin());
}

void Frame::setPayload(const Message& payload) {
  mPayload = payload.Data;
  setPayloadSize(static_cast<uint8_t>(payload.Size));
}

void Frame::setPayloadCrc(const uint8_t crc) {
  mPayloadCrc = crc;
}

const HeaderBuffer& Frame::getHeaderBuffer() const {
  return mHeader;
}

uint8_t Frame::getPayloadSize() const {
  return mHeader[PAYLOAD_SIZE_INDEX];
}

uint8_t Frame::getHeaderCrc() const {
  return mHeader[HEADER_CRC_INDEX];
}

const MessageBuffer& Frame::getPayloadBuffer() const {
  return mPayload;
}

const FrameBuffer Frame::getFrameBuffer() const {
  FrameBuffer buffer;
  std::copy(mHeader.begin(), mHeader.end(), buffer.begin());
  std::copy(mPayload.begin(), mPayload.end(), buffer.begin() + HEADER_SIZE);
  buffer[static_cast<size_t>(HEADER_SIZE) + getPayloadSize()] = mPayloadCrc;
  return buffer;
}

uint8_t Frame::getPayloadCrc() const {
  return mPayloadCrc;
}

uint8_t Frame::getSize() const {
  return HEADER_SIZE + getPayloadSize() + FOOTER_SIZE;
}
