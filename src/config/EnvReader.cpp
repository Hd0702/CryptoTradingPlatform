//
// Created by Hayden Donofrio on 3/3/23.
//
#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>

#include "EnvReader.hpp"

namespace Env {
    EnvReader::EnvReader(const std::string&& file_path) {
        std::ifstream file(file_path, std::ifstream::in);
        std::string line;

        while (std::getline(file, line)) {
            std::size_t pos = line.find('=');
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);

            mapping[key] = value;
        }
        file.close();
    }

    std::string EnvReader::operator[](std::string key) const {
        return mapping.at(key);
    }
}
