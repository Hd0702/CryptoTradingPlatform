#pragma once

#include <string>
#include <vector>

namespace Encryption {
     std::string B64Encode(std::string data);
     std::string B64Decode(std::string data);
     std::string  HMACSha512(std::string data, std::string key);
}