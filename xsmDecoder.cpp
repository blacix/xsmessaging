#include "xsmDecoder.h"

#include "xsmCRC.h"
#include "xsmUtils.h"

using namespace xsm;

Decoder::Decoder(MessageCallback callback) :
    mState(State::DELIMITER),
    mCallback(callback),
    mPrevByte(FRAME_DELIMITER),
    mDiscardedBytes(0) {
  mPotentialPayload.Size = 0;
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
    mHeader[0] = byte;
    mState = State::SIZE;
  }
}

void Decoder::receiveSize(const uint8_t byte) {
  if (byte <= MAX_PAYLOAD_SIZE) {
    mHeader[PAYLOAD_SIZE_INDEX] = byte;
    mState = State::CRC_HEADER;
  } else {
    reset();
  }
}

void Decoder::receiveHeaderCrc(const uint8_t byte) {
  if (crc8(mHeader.data(), HEADER_SIZE - 1) == byte) {
    mHeader[HEADER_CRC_INDEX] = byte;
    mState = State::PAYLOAD;
  } else {
    reset();
  }
}

void Decoder::receivePayload(const uint8_t byte) {
  if (mPotentialPayload.Size < mHeader[PAYLOAD_SIZE_INDEX]) {
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
  uint8_t calulatedCrc = crc8(mPotentialPayload.Data.data(), mHeader[PAYLOAD_SIZE_INDEX]);
  if (calulatedCrc == byte) {
    mCallback(mPotentialPayload);
  }

  reset();
}

void Decoder::payloadToCrcPayload(const uint8_t byte) {
  mPotentialPayload.Data[mPotentialPayload.Size] = byte;
  ++mPotentialPayload.Size;
  mPrevByte = byte;

  if (mPotentialPayload.Size >= mHeader[PAYLOAD_SIZE_INDEX]) {
    mState = State::CRC_PAYLOAD;
  }
}

void Decoder::reset() {
  mState = State::DELIMITER;
  mPotentialPayload.Size = 0;
  mPrevByte = FRAME_DELIMITER;
}
