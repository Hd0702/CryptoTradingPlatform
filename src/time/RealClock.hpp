#pragma once
#include "IClock.hpp"

class RealClock : public IClock {
public:
    [[nodiscard]] std::chrono::system_clock::time_point now() const override {
        return std::chrono::system_clock::now();
    }
};
