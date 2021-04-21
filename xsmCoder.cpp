#include "xsmCoder.h"

#include "xsmCRC.h"
#include "xsmUtils.h"

using namespace xsm;

const Frame Coder::encode(const Message& payload) {
  // TODO move this to Frame constructor
  Frame frame;
  frame.setEscapedPayload(payload);
  frame.setHeaderCrc(crc8(frame.getData().data(), HEADER_SIZE - 1));
  frame.setPayloadCrc(crc8(payload.Data.data(), payload.Size));
  return frame;
}
