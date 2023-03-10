#pragma once

#include <string>
#include <vector>

namespace Encryption {
     std::string B64Encode(std::string data);
     std::string B64Decode(std::string data);
     std::string HMACSha512(std::string data, std::string key);
     std::string HMACSha256(std::string data, std::string key, unsigned char *result, unsigned int *result_len);
     std::vector<unsigned char> HMACSha256(const std::string& data);
     std::string ToHex(std::string data, size_t digest_len);
}