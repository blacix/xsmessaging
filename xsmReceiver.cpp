#include "xsmReceiver.h"

#include "xsmCRC.h"
#include "xsmUtils.h"

using namespace xsm;

Receiver::Receiver(const Escaping escaping, IReceiver& callback) :
    mEscaping(escaping),
    mCallback(callback),
    mState(State::DELIMITER),
    mPrevByte(FRAME_DELIMITER),
    mDiscardedBytes(0) {
  mPotentialPayload.Size = 0;
  mFrame.setHeaderCrc(0);
  mFrame.setPayloadSize(0);
}

void Receiver::receive(const uint8_t* bytes, const size_t size) {
  for (size_t i = 0; i < size; ++i) {
    receive(bytes[i]);
  }
}

void Receiver::receive(const uint8_t byte) {
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

void Receiver::receiveDelimiter(const uint8_t byte) {
  if (byte == FRAME_DELIMITER2 && mPrevByte == FRAME_DELIMITER) {
    // Frame sets this in contructor
    mState = State::SIZE;
  }
  mPrevByte = byte;
}

void Receiver::receiveSize(const uint8_t byte) {
  if (byte <= MAX_PAYLOAD_SIZE) {
    mFrame.setPayloadSize(byte);
    mState = State::CRC_HEADER;
  } else {
    reset();
  }
}

void Receiver::receiveHeaderCrc(const uint8_t byte) {
  if (crc8(mFrame.getHeaderBuffer().data(), HEADER_SIZE - 1) == byte) {
    mFrame.setHeaderCrc(byte);
    mState = State::PAYLOAD;
  } else {
    reset();
  }
}

void Receiver::receivePayload(const uint8_t byte) {
  if (mPotentialPayload.Size < mFrame.getPayloadSize()) {
    if (mEscaping == Escaping::ON) {
      if (mPrevByte == ESCAPE_BYTE) {
        // escaped byte
        storePayload(byte);
      } else {
        // not escaped
        if (byte == FRAME_DELIMITER) {
          // not escaped FRAME_DELIMITER -> start of a new frame
          // discard the current one
          reset();
          // handle receiving the delimiter
          receiveDelimiter(byte);
        } else if (byte == FRAME_DELIMITER2) {
          // not escaped FRAME_DELIMITER2 -> something's wrong
          reset();
        } else {
          storePayload(byte);
        }
      }
    } else {
      storePayload(byte);
    }
  } else {
    reset();
  }
}

void Receiver::receivePayloadCrc(const uint8_t byte) {
  mPayloadCrc = crc8(mPotentialPayload.Data.data(), mFrame.getPayloadSize());
  if (mPayloadCrc == byte) {
    mFrame.setEscapedPayload(mPotentialPayload);
    mFrame.setPayloadCrc(mPayloadCrc);
    processFrame();
  }

  reset();
}

void Receiver::storePayload(const uint8_t byte) {
  mPotentialPayload.Data[mPotentialPayload.Size] = byte;
  ++mPotentialPayload.Size;
  mPrevByte = byte;

  if (mPotentialPayload.Size >= mFrame.getPayloadSize()) {
    mState = State::CRC_PAYLOAD;
  }
}

void Receiver::reset() {
  mState = State::DELIMITER;
  mPrevByte = FRAME_DELIMITER;
  mPotentialPayload.Size = 0;
  mFrame.setHeaderCrc(0);
  mFrame.setPayloadSize(0);
}


void Receiver::processFrame() {
  Message message;
  if (mEscaping == Escaping::ON) {
    message.Size = Utils::unescape(mFrame.getPayloadBuffer(), mFrame.getPayloadSize(), message.Data);
    mCallback.onMessageReceived(message);
  } else {
    message.Size = mFrame.getPayloadSize();
    message.Data = mFrame.getPayloadBuffer();
    mCallback.onMessageReceived(message);
  }
}