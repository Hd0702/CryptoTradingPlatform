#pragma once
#include <string>
#include "../exchanges/BaseExchange.hpp"

class BaseStrategy {
public:
    virtual bool buy() = 0;
    virtual bool sell() = 0;
    // Todo: add a way to load in strategy history and if any triggers were hit since the last trade.
protected:
    explicit BaseStrategy(std::shared_ptr<BaseExchange> exchange_): exchange(std::move(exchange_)) {}
    std::shared_ptr<BaseExchange> exchange;
    std::string pair;
    double capital;
};