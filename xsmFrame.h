#ifndef _XSM_FRAME_H
#define _XSM_FRAME_H

#include "xsmTypes.h"

namespace xsm {

class Frame {
public:

  Frame();

  void setPayloadSize(const uint8_t length);
  void setHeaderCrc(const uint8_t crc);
  void setEscapedPayload(const PayloadBuffer& payload);
  void setPayloadCrc(const uint8_t crc);

  uint8_t getPayloadSize() const;
  uint8_t getHeaderCrc() const;
  PayloadBuffer getPayload() const;
  const PacketBuffer& getData() const;
  uint8_t getPayloadCrc() const;

private:
  PacketBuffer mData;
};

} // namespace xsm

#endif