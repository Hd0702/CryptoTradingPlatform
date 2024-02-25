#pragma once
#include <string>
#include <vector>
#include <memory>
#include "BaseOHLC.hpp"

// TODO: Deprecate this class
class BaseExchange {
public:
    [[nodiscard]] virtual std::vector<std::unique_ptr<BaseOHLC>> getOHLC(const long long epochNanos, const std::string& pair) const = 0;
    [[nodiscard]] virtual double getTicker(const std::string& pair) const = 0;
private:
    std::string name;
};