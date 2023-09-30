#pragma once

#include <string>
#include <vector>

namespace Encryption {
     std::string b64Encode(std::string data);
     std::string b64Decode(std::string data);
     std::string hmacSha512(const std::string data, const std::string key);
     std::string hmacSha256(std::string data, std::string key, unsigned int *result_len);
     std::vector<unsigned char> hmacSha256(const std::string& data);
     std::string toHex(std::string data, size_t digest_len);
}