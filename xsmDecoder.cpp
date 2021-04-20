#include "xsmDecoder.h"

#include "xsmCRC.h"
#include "xsmUtils.h"

using namespace xsm;

Decoder::Decoder(IFrameCallback& callback) :
    mState(State::DELIMITER),
    mCallback(callback),
    mPrevByte(FRAME_DELIMITER),
    mDiscardedBytes(0) {
  mPotentialPayload.Size = 0;
  mFrame.setHeaderCrc(0);
  mFrame.setPayloadSize(0);
}

void Decoder::receive(const uint8_t byte) {
  switch (mState) {
    case State::DELIMITER:
      receiveDelimiter(byte);
      break;

    case State::SIZE:
      receiveSize(byte);
      break;

    case State::CRC_HEADER:
      receiveHeaderCrc(byte);
      break;

    case State::PAYLOAD:
      receivePayload(byte);
      break;

    case State::CRC_PAYLOAD:
      receivePayloadCrc(byte);
      break;

    default:
      break;
  }
}

void Decoder::receiveDelimiter(const uint8_t byte) {
  if (byte == FRAME_DELIMITER) {
    // Frame sets this in contructor
    mState = State::SIZE;
  }
}

void Decoder::receiveSize(const uint8_t byte) {
  if (byte <= MAX_PAYLOAD_SIZE) {
    mFrame.setPayloadSize(byte);
    mState = State::CRC_HEADER;
  } else {
    reset();
  }
}

void Decoder::receiveHeaderCrc(const uint8_t byte) {
  if (crc8(mFrame.getHeaderBuffer().data(), HEADER_SIZE - 1) == byte) {
    mFrame.setHeaderCrc(byte);
    mState = State::PAYLOAD;
  } else {
    reset();
  }
}

void Decoder::receivePayload(const uint8_t byte) {
  if (mPotentialPayload.Size < mFrame.getPayloadSize()) {
    bool escaped = mPrevByte == ESCAPE_BYTE;

    if (escaped) {
      // escaped byte
      escaped = false;

      payloadToCrcPayload(byte);
    } else {
      // not escaped
      if (byte == FRAME_DELIMITER) {
        // not escaped frame delimiter -> start of a new frame
        // discard the current one
        reset();
        // handle receiving the delimiter
        receiveDelimiter(byte);
      } else {
        if (byte == ESCAPE_BYTE) {
          escaped = true;
        }
        payloadToCrcPayload(byte);
      }
    }
  } else {
    reset();
  }
}

void Decoder::receivePayloadCrc(const uint8_t byte) {
  mPayloadCrc = crc8(mPotentialPayload.Data.data(), mFrame.getPayloadSize());
  if (mPayloadCrc == byte) {
    mFrame.setEscapedPayload(mPotentialPayload);
    mFrame.setPayloadCrc(mPayloadCrc);
    mCallback.onFrameReceived(mFrame);
  }

  reset();
}

void Decoder::payloadToCrcPayload(const uint8_t byte) {
  mPotentialPayload.Data[mPotentialPayload.Size] = byte;
  ++mPotentialPayload.Size;
  mPrevByte = byte;

  if (mPotentialPayload.Size >= mFrame.getPayloadSize()) {
    mState = State::CRC_PAYLOAD;
  }
}

void Decoder::reset() {
  mState = State::DELIMITER;
  mPrevByte = FRAME_DELIMITER;
  mPotentialPayload.Size = 0;
  mFrame.setHeaderCrc(0);
  mFrame.setPayloadSize(0);
}
