#ifndef NOWTECH_SERIALIZER_BUFFER_INCLUDED
#define NOWTECH_SERIALIZER_BUFFER_INCLUDED

#include "serializer.h"

namespace nowtech {

/// Interface for using Serializer with a memory buffer.
class SerializerBuffer final : public SerializerInterface {
  friend class Serializer;

private:
  /// Buffer to use for (de)serialization.
  uint8_t * const mBuffer;

  /// Capacity of the buffer.
  uint32_t const mBufferCapacity;

  /// Index of next read or write regarding the buffer.
  uint32_t mBufferIndex = 0;

  /// Start of Serializer content in buffer
  uint32_t mBufferOffset = 0u;

  /// Length of available length for de-serialization.
  uint32_t mAvailable = 0u;

  /// True if buffer overflow would have occured once since reset.
  bool mFailed = false;

public:
  /// Constructs a new instance using the given buffer and its length. The instance won't calculate, insert or check checksums.
  SerializerBuffer(uint8_t *aBuffer, uint32_t const aBufferCapacity) noexcept
    : SerializerInterface(false)
    , mBuffer(aBuffer)
    , mBufferCapacity(aBufferCapacity) {
  }

  /// Constructs a new instance using the given buffer and its length.
  /// @param aNeedsChecksum indicates whether the instance should calculate, insert or check checksums.
  SerializerBuffer(uint8_t *aBuffer, uint32_t const aBufferCapacity, bool aNeedsChecksum) noexcept
    : SerializerInterface(aNeedsChecksum)
    , mBuffer(aBuffer)
    , mBufferCapacity(aBufferCapacity) {
  }

  /// Constructs a new instance using the given buffer and its length. The instance won't calculate, insert or check checksums.
  SerializerBuffer(uint8_t *aBuffer, uint32_t const aBufferCapacity, uint32_t const aBufferOffset) noexcept
    : SerializerInterface(false)
    , mBuffer(aBuffer)
    , mBufferCapacity(aBufferCapacity)
    , mBufferOffset(aBufferOffset) {
  }

  /// Constructs a new instance using the given buffer and its length.
  /// @param aNeedsChecksum indicates whether the instance should calculate, insert or check checksums.
  SerializerBuffer(uint8_t *aBuffer, uint32_t const aBufferCapacity, bool aNeedsChecksum, uint32_t const aBufferOffset) noexcept
    : SerializerInterface(aNeedsChecksum)
    , mBuffer(aBuffer)
    , mBufferCapacity(aBufferCapacity)
    , mBufferOffset(aBufferOffset) {
  }

  /// @return true if the instance failed somehow, like checksum error or buffer overflow.
  virtual bool operator!() const noexcept override {
    return mFailed;
  }

  /// Changes the buffer offset on the fly, without respect of the actual index or other status.
  void setOffset(uint32_t const aBufferOffset) noexcept {
    mBufferOffset = aBufferOffset;
  }

  /// @return the buffer, used after serializing to transmit the serialized stuff.
  uint8_t const * getData() const noexcept {
    return mBuffer;
  }

  /// @return the length of the buffer contents.
  virtual uint32_t const getLength() const noexcept override {
    return mBufferIndex;
  }

  /// Returns the number of available bytes to write till the buffer end.
  /// Returns 0 if the object failed.
  virtual uint32_t getAvailableWrite() const noexcept override {
    return mFailed ? 0 : mBufferCapacity - mBufferIndex - mBufferOffset;
  }

  /// Returns the number of available bytes to read.
  /// Returns 0 if the object failed.
  virtual uint32_t getAvailableRead() const noexcept override {
    return mFailed ? 0 : mAvailable - mBufferIndex;
  }

  /// Sets the consumable length of the buffer for de-serialization
  virtual void setAvailablaLength(uint32_t const aAvailable) noexcept override {
    mAvailable = aAvailable;
  }

  /// Returns true if the de-serialization failed because of too short input.
  virtual bool hasRunOut() const noexcept override {
    return mFailed;
  }

  /// If length fits in the total capacity of the buffer, deletes
  /// failed flag, resets bufferIndex, and copies the content into the
  /// buffer. Otherwise sets failed.
  /// This function is used before deserializing the transmitted stuff.
  /// @param aContent the already serialized stuff to write into the buffer
  /// from its very beginning.
  /// @param aLength of the serialized stuff. If this is smaller than the
  /// buffer length, the getAvailable call will report wrong values and allows
  /// the buffer to be read past the data contents.
  void setData(uint8_t const * const aContent, uint32_t const aLength) noexcept;

protected:
  /// See the superclass.
  virtual void reset() noexcept override {
    resetChecksum();
    mBufferIndex = 0;
    mFailed = false;
  }

  virtual void skip(uint32_t aCount) noexcept override {
    mBufferIndex += aCount;
    mFailed = mFailed || (mBufferIndex + mBufferOffset >= mBufferCapacity) || mBufferIndex >= mAvailable;
  }

  /// See the superclass.
  virtual void write(uint8_t const aByte) noexcept override;

  /// See the superclass.
  virtual void writeLittleEndian(uint8_t const * const aBytes, uint32_t const aLength) noexcept override;

  /// See the superclass.
  virtual void writeBigEndian(uint8_t const * const aBytes, uint32_t const aLength) noexcept override;

  /// See the superclass.
  virtual uint8_t read() noexcept override;

  /// See the superclass.
  virtual void readLittleEndian(uint8_t * const aBytes, uint32_t const aLength) noexcept override;

  /// See the superclass.
  virtual void readBigEndian(uint8_t * const aBytes, uint32_t const aLength) noexcept override;
};

}
#endif
