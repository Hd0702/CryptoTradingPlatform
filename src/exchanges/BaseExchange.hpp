#pragma once
#include <string>
#include <vector>
#include "BaseOHLC.hpp"

class BaseExchange {
public:
    virtual std::vector<std::unique_ptr<BaseOHLC>> getOHLC(const long long epochNanos, const std::string& pair) const = 0;
    virtual std::string buy(const std::string& pair, const std::string& volume, const std::string& type, const std::string& orderType) const = 0;
    virtual double getTicker(const std::string& pair) const = 0;
private:
    std::string name;
};