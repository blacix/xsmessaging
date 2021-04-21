#ifndef _XSM_FRAME_H
#define _XSM_FRAME_H

#include "xsmTypes.h"

namespace xsm {


class Frame {
public:
  Frame();
  Frame(const Message& payload);
  void setPayloadSize(const uint8_t length);
  void setHeaderCrc(const uint8_t crc);
  void setPayloadBuffer(const MessageBuffer& payload);
  void setPayload(const Message& payload);
  void setPayloadCrc(const uint8_t crc);

  const HeaderBuffer& getHeaderBuffer() const;
  uint8_t getPayloadSize() const;
  uint8_t getHeaderCrc() const;
  const MessageBuffer& getPayloadBuffer() const;
  const FrameBuffer getFrameBuffer() const;
  uint8_t getPayloadCrc() const;
  uint8_t getSize() const;

private:
  MessageBuffer mPayload;
  HeaderBuffer mHeader;
  uint8_t mPayloadCrc;
};

class IFrameCallback {
public:
  virtual void onFrameReceived(const Frame& frame) = 0;
};

} // namespace xsm

#endif
