#include "serializerbuffer.h"

void nowtech::SerializerBuffer::setData(uint8_t const * const aContent, uint32_t const aLength) noexcept {
  if(aLength + mBufferOffset <= mBufferCapacity) {
    mFailed = false;
    for(uint32_t i = 0; i < aLength; ++i) {
      mBuffer[i + mBufferOffset] = aContent[i];
    }
    mBufferIndex = 0;
    mAvailable = aLength;
  }
  else {
    mFailed = true;
  }
}

void nowtech::SerializerBuffer::write(uint8_t const aByte) noexcept {
  if(getAvailableWrite() >= sizeof(aByte)) {
    mBuffer[mBufferIndex + mBufferOffset] = aByte;
    calculateChecksum(aByte);
    ++mBufferIndex;
  }
  else {
    mFailed = true;
  }
}

void nowtech::SerializerBuffer::writeLittleEndian(uint8_t const * const aBytes, uint32_t const aLength) noexcept {
  if(getAvailableWrite() >= aLength) {
    for(uint32_t i = 0; i < aLength; i++) {
      mBuffer[mBufferIndex + mBufferOffset] = aBytes[i];
      calculateChecksum(aBytes[i]);
      ++mBufferIndex;
    }
  }
  else {
    mFailed = true;
  }
}

void nowtech::SerializerBuffer::writeBigEndian(uint8_t const * const aBytes, uint32_t const aLength) noexcept {
  if(getAvailableWrite() >= aLength) {
    if(aLength > 0) {
      for(uint32_t i = aLength - 1; i > 0; i--) {
        mBuffer[mBufferIndex + mBufferOffset] = aBytes[i];
        calculateChecksum(aBytes[i]);
        ++mBufferIndex;
      }
      mBuffer[mBufferIndex + mBufferOffset] = aBytes[0];
      calculateChecksum(aBytes[0]);
      ++mBufferIndex;
    }
    else { // nothing to do
    }
  }
  else {
    mFailed = true;
  }
}

uint8_t nowtech::SerializerBuffer::read() noexcept {
  uint8_t ret = 0;
  if(getAvailableRead() >= sizeof(uint8_t)) {
    ret = mBuffer[mBufferIndex + mBufferOffset];
    calculateChecksum(ret);
    ++mBufferIndex;
  }
  else {
    mFailed = true;
  }
  return ret;
}

void nowtech::SerializerBuffer::readLittleEndian(uint8_t * const aBytes, uint32_t const aLength) noexcept {
  if(getAvailableRead() >= aLength) {
    for(uint32_t i = 0; i < aLength; i++) {
      aBytes[i] = mBuffer[mBufferIndex + mBufferOffset];
      calculateChecksum(aBytes[i]);
      ++mBufferIndex;
    }
  }
  else {
    mFailed = true;
  }
}

void nowtech::SerializerBuffer::readBigEndian(uint8_t * const aBytes, uint32_t const aLength) noexcept {
  if(getAvailableRead() >= aLength) {
    if(aLength > 0) {
      for(uint32_t i = aLength - 1; i > 0; i--) {
        aBytes[i] = mBuffer[mBufferIndex + mBufferOffset];
        calculateChecksum(aBytes[i]);
        ++mBufferIndex;
      }
      aBytes[0] = mBuffer[mBufferIndex + mBufferOffset];
      calculateChecksum(aBytes[0]);
      ++mBufferIndex;
    }
    else { // nothing to do
    }
  }
  else {
    mFailed = true;
  }
}
