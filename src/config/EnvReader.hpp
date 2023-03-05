//
// Created by Hayden Donofrio on 3/3/23.
//

#pragma once

#include <map>
#include <string>

namespace Env {
    class EnvReader {
    public:
        explicit EnvReader(const std::string&& file_path);
        std::string operator [](std::string key) const;
    private:
        std::map<std::string, std::string> mapping;
    };
}
