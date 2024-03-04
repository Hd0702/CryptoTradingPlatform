#pragma once
#include <chrono>

class IClock {
public:
    virtual ~IClock() = default;
    [[nodiscard]] virtual std::chrono::system_clock::time_point now() const = 0;
};