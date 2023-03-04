//
// Created by Hayden Donofrio on 3/3/23.
//

#ifndef CRYPTOTRADINGPROJECT_ENVREADER_H
#define CRYPTOTRADINGPROJECT_ENVREADER_H

#include <map>
#include <string>

namespace Env {
    class EnvReader {
    public:
        EnvReader(const std::string&& file_path);
        std::string operator [](std::string key) const;
    private:
        std::map<std::string, std::string> mapping;
    };
}
#endif //CRYPTOTRADINGPROJECT_ENVREADER_H
