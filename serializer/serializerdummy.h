#ifndef NOWTECH_SERIALIZERERDUMMY_H_INCLUDED
#define NOWTECH_SERIALIZERERDUMMY_H_INCLUDED

#include "serializer.h"
#include <limits>

namespace nowtech {

/// Interface for using a dummy Serializer.
/// This will be able to return only the serialized length.
class SerializerDummy final : public SerializerInterface {
  friend class Serializer;

  /// Index of next read or write regarding the buffer.
  uint32_t mBufferIndex = 0u;

public:
  /// Constructs a new dummy serializer interface.
  SerializerDummy() noexcept : SerializerInterface(false) {
  }

  /// Constructs a new dummy serializer interface.
  SerializerDummy(bool const aNeedsChecksum) noexcept : SerializerInterface(aNeedsChecksum) {
  }

  /// @return true if the instance failed somehow, like checksum error or buffer overflow.
  virtual bool operator!() const noexcept override {
    return false;
  }

  /// @return the length of the buffer contents.
  virtual uint32_t const getLength() const noexcept override {
    return mBufferIndex;
  }

  /// Returns std::numeric_limits<uint32_t>::max();
  virtual uint32_t getAvailableWrite() const noexcept override {
    return std::numeric_limits<uint32_t>::max();
  }

  /// Returns std::numeric_limits<uint32_t>::max();
  virtual uint32_t getAvailableRead() const noexcept override {
    return std::numeric_limits<uint32_t>::max();
  }

  virtual void setAvailablaLength(uint32_t const) noexcept override {
  }

  /// Returns true if the de-serialization failed because of too short input.
  virtual bool hasRunOut() const noexcept override {
    return false;
  }

protected:
  /// See the superclass.
  virtual void reset() noexcept override {
    mBufferIndex = 0u;
  }

  virtual void skip(uint32_t aCount) noexcept override {
    mBufferIndex += aCount;
  }

  /// See the superclass.
  virtual void write(uint8_t const aByte) noexcept override {
    mBufferIndex += 1u;
  }

  /// See the superclass.
  virtual void writeLittleEndian(uint8_t const * const aBytes, uint32_t const aLength) noexcept override {
    mBufferIndex += aLength;
  }

  /// See the superclass.
  virtual void writeBigEndian(uint8_t const * const aBytes, uint32_t const aLength) noexcept override {
    mBufferIndex += aLength;
  }

  /// See the superclass.
  virtual uint8_t read() noexcept override {
    mBufferIndex += 1u;
    return 0u;
  }

  /// See the superclass.
  virtual void readLittleEndian(uint8_t * const aBytes, uint32_t const aLength) noexcept override {
    mBufferIndex += aLength;
  }

  /// See the superclass.
  virtual void readBigEndian(uint8_t * const aBytes, uint32_t const aLength) noexcept override {
    mBufferIndex += aLength;
  }
};

}

#endif /* NOWTECH_SERIALIZERERDUMMY_H_INCLUDED */
