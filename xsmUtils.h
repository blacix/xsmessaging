#ifndef _UTILS_H
#define _UTILS_H
#include <string>
#include <vector>
namespace xsm
{
class xsmUtils
{
public:
	xsmUtils() = default;
	~xsmUtils() = default;

	// helper method to convert hex string to byte
	static uint8_t hexStringToByte(const std::string& hexString);
	// helper method to convert hex string to byte array
	static std::vector<uint8_t> hexStringToByteArray(const std::string& hexString);
	// helper method to get epoch timestamp ms
	static long long getCurrentTimestamp();
};
}
#endif
