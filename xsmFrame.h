#ifndef _XSM_FRAME_H
#define _XSM_FRAME_H

#include "xsmTypes.h"

namespace xsm {

class Frame {
public:

  Frame();

  void setPayloadSize(const uint8_t length);
  void setHeaderCrc(const uint8_t crc);
  void setEscapedPayload(const MessageBuffer& payload);
  void setPayloadCrc(const uint8_t crc);

  uint8_t getPayloadSize() const;
  uint8_t getHeaderCrc() const;
  MessageBuffer getPayload() const;
  const PacketBuffer& getData() const;
  uint8_t getPayloadCrc() const;
  uint8_t getSize() const;

private:
  PacketBuffer mData;
};

} // namespace xsm

#endif