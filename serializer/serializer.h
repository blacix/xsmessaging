#ifndef NOWTECH_SERIALIZER_INCLUDED
#define NOWTECH_SERIALIZER_INCLUDED

#include "BanCopyMove.h"
#include <algorithm>
#include <cstdint>
#include <limits>

namespace nowtech {

/// Class for determining and holding endian info.
class EndianInfo final {
private:
  /// True if we are on a little-endian architecture.
  static bool littleEndian;

public:
  /// Sets endianness for future use.
  ///  X86 and x64 are little-endian. Called in a static instance of class
  /// InitStatic to register built-in types.
  static void initStatic() noexcept;

  /// Returns true if we are on little-endian arch.
  static bool isLittle() noexcept {
    return littleEndian;
  }
};

class Crc16 : public BanCopyMove {
public:
  static constexpr uint16_t cCrc16size = 2;

private:
  /// Needed for checksum calculation.
  static uint16_t constexpr cPrimeTable[16] = {0x049D, 0x0C07, 0x1591, 0x1ACF, 0x1D4B, 0x202D, 0x2507, 0x2B4B, 0x34A5, 0x38C5, 0x3D3F, 0x4445, 0x4D0F, 0x538F, 0x5FB3, 0x6BBF};

  /// Actual prime index for checksum calculation.
  uint8_t mPrimeIndex = 0;

protected:
  /// Checksum calculated from reset or creation.
  uint16_t mChecksum = 0;

public:
  static uint16_t crc16(const void *buffer, uint32_t size) noexcept {
    return crc16(buffer, 0u, size);
  }

  static uint16_t crc16(const void *buffer, uint32_t offset, uint32_t size) noexcept {
    uint16_t result = 0;
    const uint8_t *p = (const uint8_t*)buffer + offset;
    for(uint32_t i=0; i<size; i++) {
      result += (*p++ ^ 0x5A) * cPrimeTable[i & 0x0F];
    }
    return result;
  }

  /// Resets the checksum calculation.
  void resetChecksum() noexcept {
    mChecksum = 0u;
    mPrimeIndex = 0u;
  }

  /// Returns the checksum so far.
  uint16_t getChecksum() const noexcept {
    return mChecksum;
  }

  /// Uses a simple 16-bit checksum calculation algorithm taken from
  /// Cybro PLC protocols.
  void calculateChecksum(uint8_t const aByte) noexcept {
    mChecksum += (aByte ^ 0x5A) * cPrimeTable[mPrimeIndex & 0x0F];
    ++mPrimeIndex;
  }

};

class Serializer;
class EmbeddedRandomSerializer;

/// Interface for writing and reading during serialisation and calculating
/// checksum. A class implementing this interface is needed for Serializer to work.
/// It checks for buffer / storage overflow before each operation and if
/// it would happen or had happened in a previous operation, does nothing,
/// but remembers it in a fail flag.
/// Reading and writing little- or big-endian multibyte types is supported.
/// IMPORTANT: storage is always done in little-endian.
class SerializerInterface : public BanCopyMove {

  // Friendship is needed to be able to hide read and write method from the API.
  friend class Serializer;
  friend class EmbeddedRandomSerializer;
protected:
  Crc16 mCrc;
  bool mNeedsChecksum = false;

  SerializerInterface(bool aNeedsChecksum) : mNeedsChecksum(aNeedsChecksum) {
  }

public:
  /// Resets the checksum calculation.
  void resetChecksum() noexcept {
    if(mNeedsChecksum) {
      mCrc.resetChecksum();
    }
    else { // nothing to do
    }
  }

  /// Checks (if needed) if the current checksum matches the uint16_t value
  /// to be read at the moment.
  /// Note, the underlying read operations alter the checksum,
  /// so it should not be used afterwards.
  void calculateChecksum(uint8_t const aByte) noexcept {
    if(mNeedsChecksum) {
      mCrc.calculateChecksum(aByte);
    }
    else { // nothing to do
    }
  }

  /// Appends the current checksum to the end of the storage.
  /// Note, the underlying write operations alter the checksum,
  /// so it should not be used afterwards.
  void writeChecksum() noexcept {
    if(mNeedsChecksum) {
      uint16_t checksum = mCrc.getChecksum();
      write(checksum & 255);
      write(checksum >> 8);
    }
    else { // nothing to do
    }
  }

  /// Checks if the current checksum matches the uint16_t value
  /// to be read at the moment.
  /// Note, the underlying read operations alter the checksum,
  /// so it should not be used afterwards.
  bool checkChecksum() noexcept {
    if(mNeedsChecksum) {
      uint16_t checksum = mCrc.getChecksum();
      uint16_t storedLow = read();
      uint16_t storedHigh = read();
      return checksum == storedLow + (storedHigh << 8);
    }
    else { // nothing to do
      return true;
    }
  }

  /// Returns true if the serialization failed some time.
  virtual bool operator!() const noexcept = 0;

  /// Changes the buffer offset on the fly, without respect of the actual index or other status.
  void setOffset(uint32_t const aBufferOffset) noexcept {
  }

  /// @return the length of the buffer contents so far.
  virtual uint32_t const getLength() const noexcept = 0;

  /// Returns the available space in the container, if known.
  /// Returns numeric_limits<uint32_t>::max() if not known.
  /// Returns 0 if already failed.
  virtual uint32_t getAvailableWrite() const noexcept = 0;

  /// Returns the number of available bytes to read.
  /// Returns 0 if the object failed.
  virtual uint32_t getAvailableRead() const noexcept = 0;

  /// Sets the consumable length of the buffer for de-serialization.
  virtual void setAvailablaLength(uint32_t const aAvailable) noexcept = 0;

  /// Returns true if the de-serialization failed because of too short input.
  virtual bool hasRunOut() const noexcept = 0;

protected:
  /// Resets the storage, including checksum calculation
  /// and the fail flag.
  /// Needed for new (de)serialisation sessions.
  virtual void reset() noexcept = 0;

  /// Skips the given amount of bytes.
  virtual void skip(uint32_t aCount) noexcept = 0;

  /// Writes a byte and calculates its checksum.
  virtual void write(uint8_t const aByte) noexcept = 0;

  /// Writes a little-endian memory region and calculates its checksum.
  /// Also used to write strings.
  /// @param aBytes start of region
  /// @param aLength length of stuff to write
  virtual void writeLittleEndian(uint8_t const * const aBytes, uint32_t const aLength) noexcept = 0;

  /// Writes a big-endian memory region in little-endian format
  /// and calculates its checksum.
  /// @param aBytes start of region
  /// @param aLength length of stuff to write
  virtual void writeBigEndian(uint8_t const * const aBytes, uint32_t const aLength) noexcept = 0;

  /// Reads a byte and calculates its checksum.
  /// @returns the byte read.
  virtual uint8_t read() noexcept = 0;

  /// Reads a little-endian memory region and calculates its checksum.
  /// Also used to read strings.
  /// @param aBytes start of region
  /// @param aLength length of stuff to read
  virtual void readLittleEndian(uint8_t * const aBytes, uint32_t const aLength) noexcept = 0;

  /// Reads into a big-endian memory region from little-endian format.
  /// and calculates its checksum
  /// @param aBytes start of region
  /// @param aLength length of stuff to read
  virtual void readBigEndian(uint8_t * const aBytes, uint32_t const aLength) noexcept = 0;
};


class Serializer : public BanCopyMove {
protected:
  static constexpr uint16_t cIllegalId  = std::numeric_limits<uint16_t>::max();
  static constexpr uint8_t  cMagicFirst = 0x55;
  static constexpr uint8_t  cMagicLast  = 0xaa;

  /// The interface to access the storage medium.
  SerializerInterface &mStorageInterface;

private:
  /// Datastructure revision number of the current application
  uint16_t const mActualFirmwareStructureRevision;

  /// ID of the current application
  uint16_t const mActualFirmwareApplicationId;

  /// Datastructure revision number of the stored data
  uint16_t mStoredStructureRevision = cIllegalId;

  /// Application ID used when storing the data
  uint16_t mStoredApplicationId = cIllegalId;

  bool mFailed = false;

public:
  /// Constructs a new instance using the given storage access interface and illegal structure revision and application ID.
  /// @param aInterface the interface
  Serializer(SerializerInterface &aInterface) noexcept
    : mStorageInterface(aInterface)
    , mActualFirmwareStructureRevision(cIllegalId)
    , mActualFirmwareApplicationId(cIllegalId) {
  }

  /// Constructs a new instance using the given storage access interface
  /// @param aInterface the interface
  /// @param aActualFirmwareStructureRevision Datastructure revision number of the current application
  /// @param aActualFirmwareApplicationId ID of the current application
  Serializer(SerializerInterface &aInterface
    , uint16_t const aActualFirmwareStructureRevision
    , uint16_t const aActualFirmwareApplicationId) noexcept
    : mStorageInterface(aInterface)
    , mActualFirmwareStructureRevision(aActualFirmwareStructureRevision)
    , mActualFirmwareApplicationId(aActualFirmwareApplicationId) {
  }

  /// @return the length of the buffer contents, if applicable,
  /// or 0 if not meaningful.
  uint32_t getLength() const noexcept {
    return mStorageInterface.getLength();
  }

  uint16_t getStoredStructureRevision() const noexcept {
    return mStoredStructureRevision;
  }

  /// Returns true if some operation failed.
  bool operator!() const noexcept {
    return mFailed || !mStorageInterface;
  }

  /// Resets storage interface.
  virtual void reset() noexcept {
    mFailed = false;
    mStorageInterface.reset();
  }

  /// Skips the given amount of bytes without calculating the CRC.
  void skipDiscardCrc(uint32_t aCount) noexcept {
    mStorageInterface.skip(aCount);
  }

  /// Skips the given amount of bytes with calculating the CRC read from the underlying interface.
  void skipCalculateCrc(uint32_t aCount) noexcept {
    for(uint32_t i = 0u; i < aCount; ++i) {
      uint8_t tmp;
      *this >> tmp;
    }
  }

  /// Writes two-byte magic constant.
  void writeMagic() noexcept {
    *this << cMagicFirst << cMagicLast;
  }

  /// Writes mActualFirmwareStructureRevision and mActualFirmwareApplicationId.
  void writeRevisionId() noexcept {
    *this << mActualFirmwareStructureRevision << mActualFirmwareApplicationId;
  }

  /// Reads the two magic bytes.
  /// Sets mFailed if they don't match.
  /// @return !mFailed;
  bool checkMagic() noexcept {
    uint8_t first, last;
    *this >> first >> last;
    bool magicFailed = first != cMagicFirst || last != cMagicLast;
    mFailed = mFailed || magicFailed;
    return !magicFailed;
  }

  /// Reads mStoredStructureRevision and mStoredApplicationId.
  /// Sets mFailed if the application IDs don't match.
  /// @return !mFailed;
  bool readStructureRevisionCheckApplicationId() noexcept {
    *this >> mStoredStructureRevision >> mStoredApplicationId;
    bool idFailed = mActualFirmwareApplicationId != mStoredApplicationId;
    mFailed = mFailed || idFailed;
    return !idFailed;
  }

  /// Calls the corresponding method in the storage interface.
  void writeChecksum() noexcept {
    mStorageInterface.writeChecksum();
  }

  /// Calls the corresponding method in the storage interface.
  void resetChecksum() noexcept {
    mStorageInterface.resetChecksum();
  }

  /// Calls the corresponding method in the storage interface.
  /// @returns true if it was OK and no previous error.
  bool checkChecksum() noexcept {
    bool checksumOk = mStorageInterface.checkChecksum();
    mFailed = mFailed || !checksumOk;
    return checksumOk;
  }

  /// Serializes the value into as 1 byte.
  Serializer& operator<<(bool const value) noexcept {
    return *this << (value ? uint8_t(1u) : uint8_t(0u));
  }

  /// Serializes the value as 1 byte.
  Serializer& operator<<(int8_t const value) noexcept;

  /// Serializes the value as 2 bytes.
  Serializer& operator<<(int16_t const value) noexcept;

  /// Serializes the value as 4 bytes.
  Serializer& operator<<(int32_t const value) noexcept;

  /// Serializes the value as 8 bytes.
  Serializer& operator<<(int64_t const value) noexcept;

  /// Serializes the value as 1 byte.
  Serializer& operator<<(uint8_t const value) noexcept;

  /// Serializes the value as 2 bytes.
  Serializer& operator<<(uint16_t const value) noexcept;

  /// Serializes the value as 4 bytes.
  Serializer& operator<<(uint32_t const value) noexcept;

  /// Serializes the value as 8 bytes.
  Serializer& operator<<(uint64_t const value) noexcept;

  /// Serializes the value as 4 bytes. Assumes the internal
  /// floating-point representation is standard.
  Serializer& operator<<(float const value) noexcept;

  /// Serializes the value as 8 bytes. Assumes the internal
  /// floating-point representation is standard.
  Serializer& operator<<(double const value) noexcept;

  /// Determines the length of the 0-delimited strings, stores its
  /// length as uint32_t and then the strings itself without the 0.
  /// @param aPtr pointer to the string.
  // TODO overload in subclass
  virtual Serializer& operator<<(char const * const aPtr) noexcept;

  /// Determines the length of the 0-delimited strings, stores its
  /// length as uint32_t and then the strings itself without the 0.
  /// @param aPtr pointer to the string.
  Serializer& operator<<(char * const aPtr) noexcept {
    return *this << const_cast<char const * const>(aPtr);
  }

  /// Writes a fixed-length chunk of memory into the buffer without
  /// first writing its length.
  /// @param aBytes start of region to write.
  /// @param aLength number of bytes to write.
  // TODO overload in subclass
  virtual Serializer& write(uint8_t const * const aBytes, uint32_t const aLength) noexcept {
    mStorageInterface.writeLittleEndian(aBytes, aLength);
    return *this;
  }

  /// Reads a bool value of 1 byte into var.
  Serializer& operator>>(bool &aVar) noexcept;

  /// Reads 1 byte into var.
  Serializer& operator>>(int8_t &var) noexcept;

  /// Reads 2 bytes into var.
  Serializer& operator>>(int16_t &var) noexcept;

  /// Reads 4 bytes into var.
  Serializer& operator>>(int32_t &var) noexcept;

  /// Reads 8 bytes into var.
  Serializer& operator>>(int64_t &var) noexcept;

  /// Reads 1 byte into var.
  Serializer& operator>>(uint8_t &var) noexcept;

  /// Reads 2 bytes into var.
  Serializer& operator>>(uint16_t &var) noexcept;

  /// Reads 4 bytes into var.
  Serializer& operator>>(uint32_t &var) noexcept;

  /// Reads 8 bytes into var.
  Serializer& operator>>(uint64_t &var) noexcept;

  /// Reads 4 bytes as float into var. Assumes the internal
  /// floating-point representation is standard.
  Serializer& operator>>(float &var) noexcept;

  /// Reads 8 bytes as double into var. Assumes the internal
  /// floating-point representation is standard.
  Serializer& operator>>(double &var) noexcept;

  /// Extracts a 0-delimited character array using the preceding uint32_t length info.
  /// Does not reserves memory, it must be enough, including the terminating 0.
  /// This call appends the terminating 0.
  // TODO overload in subclass
  virtual Serializer& operator>>(char * const aPtr) noexcept;

  /// Reads a fixed-length chunk into the memory.
  /// @param aBytes start of address to write.
  /// @param aLength number of bytes to write.
  // TODO overload in subclass
  virtual Serializer& read(uint8_t * const aBytes, uint32_t const aLength) noexcept {
    mStorageInterface.readLittleEndian(aBytes, aLength);
    return *this;
  }

protected:
  /// Writes a numeric value in its little-endian representation
  /// into the underlying storage.
  /// @param aBytes start of the value
  /// @param aLength sizeof(that value)
  /// overload in subclass
  virtual void writeEndian(uint8_t const * const aBytes, uint32_t const aLength) noexcept;

  /// Reads a numeric value from its little-endian representation
  /// from the underlying storage into the buffer considering the system endianness.
  /// @param aBytes start of the value
  /// @param aLength sizeof(that value)
  /// overload in subclass
  virtual void readEndian(uint8_t * const aBytes, uint32_t const aLength) noexcept;
};


class EmbeddedRandomProviderInterface {
public:
  virtual uint8_t const * getRandomBytes(uint8_t const aCount) noexcept = 0;
};


/// Class suitable for automatically inserting random bytes in each block of serialized data.
/// This is not intended to be used as a transparent replacement to Serializer.
/// The methods skip and those accepting pointers are not implemented here, so calling the baseclass
/// variants will break the random bytes insertion.
/// This implementation allows better performance.
/// Moreover, this implementation is ONLY working with little-endian architectures.
/// On big endian ones, the behavior is undefined.
class EmbeddedRandomSerializer : public Serializer {
private:
  EmbeddedRandomProviderInterface &mRandomInterface;
  uint8_t const cBlockSize;
  uint8_t const cRandomPadCountPerBlock;
  uint8_t mModulus = 0;
  uint8_t *mZeros;

public:
  /// Constructs a new instance using the given interfaces
  /// @param aStorageInterface the storage access interface
  /// @param aRandomStorageInterface the storage access interface
  /// @param aActualFirmwareStructureRevision Datastructure revision number of the current application
  /// @param aActualFirmwareApplicationId ID of the current application
  /// @param aBlockSize length of blocks to consider in bytes.
  /// @param aRandomPadCountPerBlock number of required random bytes per block
  EmbeddedRandomSerializer(SerializerInterface &aStorageInterface
    , uint16_t const aActualFirmwareStructureRevision
    , uint16_t const aActualFirmwareApplicationId
    , EmbeddedRandomProviderInterface &aRandomInterface
    , uint8_t aBlockSize
    , uint8_t aRandomPadCountPerBlock) noexcept
    : Serializer(aStorageInterface, aActualFirmwareStructureRevision, aActualFirmwareApplicationId)
    , mRandomInterface(aRandomInterface)
    , cBlockSize(aBlockSize)
    , cRandomPadCountPerBlock(aRandomPadCountPerBlock)
    , mZeros(new uint8_t[cBlockSize]) {
    std::fill_n(mZeros, cBlockSize, 0u);
  }

  ~EmbeddedRandomSerializer() noexcept {
    delete[] mZeros;
  }

  using Serializer::operator<<;
  using Serializer::operator>>;

  virtual void reset() noexcept override {
    Serializer::reset();
    mModulus = 0;
  }

  // Skips the given amount of bytes.
  // TODO implement if needed virtual void skip(uint32_t aCount) noexcept;

  /// Determines the length of the 0-delimited strings, stores its
  /// length as uint32_t and then the strings itself without the 0.
  /// @param aPtr pointer to the string.
  virtual Serializer& operator<<(char const * const aPtr) noexcept override;

  /// Writes a fixed-length chunk of memory into the buffer without
  /// first writing its length.
  /// @param aBytes start of region to write.
  /// @param aLength number of bytes to write.
  virtual Serializer& write(uint8_t const * const aBytes, uint32_t const aLength) noexcept override {
    writeEndian(aBytes, aLength);
    return *this;
  }

  /// Extracts a 0-delimited character array using the preceding uint32_t length info.
  /// Does not reserves memory, it must be enough, including the terminating 0.
  /// This call appends the terminating 0.
  virtual Serializer& operator>>(char * const aPtr) noexcept override;

  /// Reads a fixed-length chunk into the memory.
  /// @param aBytes start of address to write.
  /// @param aLength number of bytes to write.
  virtual Serializer& read(uint8_t * const aBytes, uint32_t const aLength) noexcept override {
    readEndian(aBytes, aLength);
    return *this;
  }

  void fillBlock() noexcept {
    if (mModulus > 0u) {
      mStorageInterface.writeLittleEndian(mZeros, cBlockSize - mModulus);
    }
    else { // nothing to do
    }
  }

protected:
  /// Writes a numeric value in its little-endian representation
  /// into the underlying storage. This function automatically inserts
  /// the necessary random bytes by calling mRandomInterface.getRandomBytes.
  /// @param aBytes start of the value
  /// @param aLength sizeof(that value)
  virtual void writeEndian(uint8_t const * const aBytes, uint32_t const aLength) noexcept override;

  /// Reads a numeric value from its little-endian representation
  /// from the underlying storage into the buffer considering the system endianness.
  /// This function automatically discards the random bytes inserted during serialization.
  /// @param aBytes start of the value
  /// @param aLength sizeof(that value)
  virtual void readEndian(uint8_t * const aBytes, uint32_t const aLength) noexcept override;

};

/// Template class to provide direct access to one specific type at the specified Offset
/// The implementation is not efficient.
template<typename OneType>
class DirectAccessSoleSerializer final : public BanCopyMove {
private:
  static constexpr uint16_t cDummy = 0u;
  Serializer           mSerializer;
  SerializerInterface &mSerializerInterface;
  uint32_t             cOffset;

public:
  DirectAccessSoleSerializer(SerializerInterface &aSerializerInterface, uint32_t const aOffset) noexcept
  : mSerializer(aSerializerInterface, cDummy, cDummy)
  , mSerializerInterface(aSerializerInterface)
  , cOffset(aOffset) {
  }

  void operator=(OneType const aValue) noexcept {
    mSerializerInterface.setOffset(cOffset);
    mSerializer.reset();
    mSerializer << aValue;
  }

  operator OneType() noexcept {
    mSerializerInterface.setOffset(cOffset);
    mSerializer.reset();
    OneType ret;
    mSerializer >> ret;
    return ret;
  }
};

} // namespace nowtech

#endif
