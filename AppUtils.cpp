#include "AppUtils.h"

#include <chrono>
#include <cinttypes>
#include <iostream>
#include <iterator>
#include <sstream>

#include "xsmTypes.h"

using namespace xsm;

uint8_t AppUtils::hexStringToByte(const std::string& hexString) {
  // the return value
  // uint8_t returnValue = 0;
  // less safe but in many implementations legacy C string functions
  // are faster than std
  // won't compile w/o _CRT_SECURE_NO_WARNINGS with vc++ compiler
  // sscanf(hexString.c_str(), "%hhx", &returnValue);

  // convert ascii hex string to byte using std
  // must use int since using char extracts the next char from the stream
  // and treated differently than other numeric types
  // unsigned int value = 0;
  // std::stringstream ss;
  // ss << std::hex << hexString;
  // ss >> value;

  // cpp11 has standard methods for this, but they operate on long types
  unsigned long value = std::stoul(hexString, nullptr, 16);
  return static_cast<uint8_t>(value);
}

std::vector<uint8_t> AppUtils::hexStringToByteArray(const std::string& hexString) {
  std::vector<uint8_t> retVal;
  std::stringstream ss(hexString);
  ss << std::hex;
  // cannot split with istream_iterator into char directly
  // because chars are not treated as numbers
  // short would do, but that should be casted to uint8_t as well
  // that would be as follows
  std::vector<unsigned short> numbers
      = std::vector<uint16_t>(std::istream_iterator<uint16_t>(ss), std::istream_iterator<uint16_t>());

  for (uint16_t s : numbers) {
    retVal.push_back(static_cast<uint8_t>(s));
  }

  // another implementation could operate on strings using the hexStringToByte helper method
  // that might be faster, but does not make much difference
  // std::vector<std::string> numbers = std::vector<std::string>(
  //	std::istream_iterator<std::string>(ss),
  //	std::istream_iterator<std::string>()
  //	);

  // for (std::string s : numbers)
  //{
  //	retVal.push_back(hexStringToByte(s));
  //}

  return retVal;
}


long long AppUtils::getCurrentTimestamp() {
  std::chrono::milliseconds ms
      = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
  return ms.count();
}

void AppUtils::print(const uint8_t* packet, size_t packetSize) {
  std::stringstream ss;
  ss << std::hex;
  for (size_t i = 0; i < packetSize; i++) {
    ss << "0x" << (int)packet[i] << " ";
  }
  std::cout << ss.str() << std::endl;
}
