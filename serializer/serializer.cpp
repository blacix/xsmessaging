#include"serializer.h"

bool nowtech::EndianInfo::littleEndian;

void nowtech::EndianInfo::initStatic() noexcept {
  union {
    uint32_t i;
    char c[4];
  } bint = {
    0x01020304
  };
  littleEndian = bint.c[0] == 4;
}

uint16_t constexpr nowtech::Crc16::cPrimeTable[];

nowtech::Serializer& nowtech::Serializer::operator<<(int8_t const value) noexcept {
  union {
    uint8_t ui;
    int8_t i;
  };
  i = value;
  return *this << ui;
}

nowtech::Serializer& nowtech::Serializer::operator<<(int16_t const value) noexcept {
  union {
    uint8_t bytes[sizeof(value)];
    int16_t i;
  };
  i = value;
  writeEndian(bytes, sizeof(value));
  return *this;
}

nowtech::Serializer& nowtech::Serializer::operator<<(int32_t const value) noexcept {
  union {
    uint8_t bytes[sizeof(value)];
    int32_t i;
  };
  i = value;
  writeEndian(bytes, sizeof(value));
  return *this;
}

nowtech::Serializer& nowtech::Serializer::operator<<(int64_t const value) noexcept {
  union {
    uint8_t bytes[sizeof(value)];
    int64_t i;
  };
  i = value;
  writeEndian(bytes, sizeof(value));
  return *this;
}

/// Serializes the value as 1 byte.
nowtech::Serializer& nowtech::Serializer::operator<<(uint8_t const value) noexcept  {
  writeEndian(&value, sizeof(value));
  return *this;
}

nowtech::Serializer& nowtech::Serializer::operator<<(uint16_t const value) noexcept {
  union {
    uint8_t bytes[sizeof(value)];
    uint16_t i;
  };
  i = value;
  writeEndian(bytes, sizeof(value));
  return *this;
}

nowtech::Serializer& nowtech::Serializer::operator<<(uint32_t const value) noexcept {
  union {
    uint8_t bytes[sizeof(value)];
    uint32_t i;
  };
  i = value;
  writeEndian(bytes, sizeof(value));
  return *this;
}

nowtech::Serializer& nowtech::Serializer::operator<<(uint64_t const value) noexcept {
  union {
    uint8_t bytes[sizeof(value)];
    uint64_t i;
  };
  i = value;
  writeEndian(bytes, sizeof(value));
  return *this;
}

nowtech::Serializer& nowtech::Serializer::operator<<(float const value) noexcept {
  union {
    uint8_t bytes[sizeof(value)];
    float i;
  };
  i = value;
  writeEndian(bytes, sizeof(value));
  return *this;
}

nowtech::Serializer& nowtech::Serializer::operator<<(double const value) noexcept {
  union {
    uint8_t bytes[sizeof(value)];
    double i;
  };
  i = value;
  writeEndian(bytes, sizeof(value));
  return *this;
}

nowtech::Serializer& nowtech::Serializer::operator<<(char const * const aPtr) noexcept {
  uint32_t length = 0;
  while(aPtr[length] != 0) {
    length++;
  }
  *this << length;
  mStorageInterface.writeLittleEndian(reinterpret_cast<uint8_t const * const>(aPtr), length);
  return *this;
}

nowtech::Serializer& nowtech::Serializer::operator>>(bool &aVar) noexcept {
  uint8_t value;
  *this >> value;
  aVar = value != 0;
  return *this;
}

nowtech::Serializer& nowtech::Serializer::operator>>(int8_t &var) noexcept {
  union {
    uint8_t ui;
    int8_t i;
  };
  *this >> ui;
  var = i;
  return *this;
}

nowtech::Serializer& nowtech::Serializer::operator>>(int16_t &var) noexcept {
  union {
    uint8_t bytes[sizeof(var)];
    int16_t i;
  };
  readEndian(bytes, sizeof(var));
  var = i;
  return *this;
}

nowtech::Serializer& nowtech::Serializer::operator>>(int32_t &var) noexcept {
  union {
    uint8_t bytes[sizeof(var)];
    int32_t i;
  };
  readEndian(bytes, sizeof(var));
  var = i;
  return *this;
}

nowtech::Serializer& nowtech::Serializer::operator>>(int64_t &var) noexcept {
  union {
    uint8_t bytes[sizeof(var)];
    int64_t i;
  };
  readEndian(bytes, sizeof(var));
  var = i;
  return *this;
}

nowtech::Serializer& nowtech::Serializer::operator>>(uint8_t &var) noexcept {
  readEndian(&var, sizeof(uint8_t));
  return *this;
}

nowtech::Serializer& nowtech::Serializer::operator>>(uint16_t &var) noexcept {
  union {
    uint8_t bytes[sizeof(var)];
    uint16_t i;
  };
  readEndian(bytes, sizeof(var));
  var = i;
  return *this;
}

nowtech::Serializer& nowtech::Serializer::operator>>(uint32_t &var) noexcept {
  union {
    uint8_t bytes[sizeof(var)];
    uint32_t i;
  };
  readEndian(bytes, sizeof(var));
  var = i;
  return *this;
}

nowtech::Serializer& nowtech::Serializer::operator>>(uint64_t &var) noexcept {
  union {
    uint8_t bytes[sizeof(var)];
    uint64_t i;
  };
  readEndian(bytes, sizeof(var));
  var = i;
  return *this;
}

nowtech::Serializer& nowtech::Serializer::operator>>(float &var) noexcept {
  union {
    uint8_t bytes[sizeof(var)];
    float i;
  };
  readEndian(bytes, sizeof(var));
  var = i;
  return *this;
}

nowtech::Serializer& nowtech::Serializer::operator>>(double &var) noexcept {
  union {
    uint8_t bytes[sizeof(var)];
    double i;
  };
  readEndian(bytes, sizeof(var));
  var = i;
  return *this;
}

nowtech::Serializer& nowtech::Serializer::operator>>(char * const aPtr) noexcept {
  uint32_t length;
  *this >> length;
  mStorageInterface.readLittleEndian(reinterpret_cast<uint8_t * const>(aPtr), length);
  if (!mStorageInterface) {
  } else {
    aPtr[length] = 0;
  }
  
  return *this;
}

void nowtech::Serializer::writeEndian(uint8_t const * const aBytes, uint32_t const aLength) noexcept {
  if(EndianInfo::isLittle()) {
    mStorageInterface.writeLittleEndian(aBytes, aLength);
  }
  else {
    mStorageInterface.writeBigEndian(aBytes, aLength);
  }
}

void nowtech::Serializer::readEndian(uint8_t * const aBytes, uint32_t const aLength) noexcept {
  if(EndianInfo::isLittle()) {
    mStorageInterface.readLittleEndian(aBytes, aLength);
  }
  else {
    mStorageInterface.readBigEndian(aBytes, aLength);
  }
}

nowtech::Serializer& nowtech::EmbeddedRandomSerializer::operator<<(char const * const aPtr) noexcept {
  uint32_t length = 0;
  while(aPtr[length] != 0) {
    length++;
  }
  *this << length;
  writeEndian(reinterpret_cast<uint8_t const * const>(aPtr), length);
  return *this;
}

nowtech::Serializer& nowtech::EmbeddedRandomSerializer::operator>>(char * const aPtr) noexcept {
  uint32_t length;
  *this >> length;
  readEndian(reinterpret_cast<uint8_t * const>(aPtr), length);
  aPtr[length] = 0;
  return *this;
}

// TODO alter these to insert random padding not on call boundaries but exactly at block starts
void nowtech::EmbeddedRandomSerializer::writeEndian(uint8_t const * const aBytes, uint32_t const aLength) noexcept {
  if(EndianInfo::isLittle()) {
    uint32_t remaining = aLength;
    uint8_t const * writeFrom = aBytes;
    while(remaining > 0) {
      if(mModulus == 0) {
        mStorageInterface.writeLittleEndian(mRandomInterface.getRandomBytes(cRandomPadCountPerBlock), cRandomPadCountPerBlock);
        mModulus += cRandomPadCountPerBlock;
      }
      uint32_t chunk = remaining;
      if(chunk > static_cast<uint32_t>(cBlockSize - mModulus)) {
        chunk = cBlockSize - mModulus;
      }
      else { // nothing to do
      }
      mStorageInterface.writeLittleEndian(writeFrom, chunk);
      remaining -= chunk;
      writeFrom += chunk;
      mModulus = (mModulus + chunk) % cBlockSize;
    }
  }
  else { // Not implemented on purpose, leave it undefined.
  }
}

void nowtech::EmbeddedRandomSerializer::readEndian(uint8_t * const aBytes, uint32_t const aLength) noexcept {
  if(EndianInfo::isLittle()) {
    uint32_t remaining = aLength;
    uint8_t * writeFrom = aBytes;
    while(remaining > 0) {
      if(mModulus == 0) {
        mStorageInterface.skip(cRandomPadCountPerBlock);
        mModulus += cRandomPadCountPerBlock;
      }
      uint32_t chunk = remaining;
      if(chunk > static_cast<uint32_t>(cBlockSize - mModulus)) {
        chunk = cBlockSize - mModulus;
      }
      else { // nothing to do
      }
      mStorageInterface.readLittleEndian(writeFrom, chunk);
      remaining -= chunk;
      writeFrom += chunk;
      mModulus = (mModulus + chunk) % cBlockSize;
    }
  }
  else { // Not implemented on purpose, leave it undefined.
  }
}
