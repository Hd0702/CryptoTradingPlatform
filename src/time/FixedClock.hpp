#pragma once
#include "IClock.hpp"
#include "time_concepts.hpp"

class FixedClock: public IClock {
    std::chrono::system_clock::time_point time;
public:
    FixedClock(const std::chrono::system_clock::time_point time): time(time) {}

    [[nodiscard]] std::chrono::system_clock::time_point now() const override {
        return time;
    }

    template<typename T> requires chrono_duration<T>
    void tick(const T& duration) {
        time += duration;
    }
};
