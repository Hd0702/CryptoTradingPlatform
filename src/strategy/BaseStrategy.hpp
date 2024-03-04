#pragma once
#include <string>
#include "../exchanges/BaseExchange.hpp"
#include "exchanges/kraken/KrakenClient.hpp"
#include "time/IClock.hpp"

class BaseStrategy {
public:
    virtual void buy() = 0;
    virtual void sell() = 0;
    // Todo: add a way to load in strategy history and if any triggers were hit since the last trade.
protected:
    // Should add a filepath for current trades
    explicit BaseStrategy(const Kraken::KrakenClient& exchange_, std::unique_ptr<IClock> clock_): exchange(exchange_), clock(std::move(clock_)) {}
    const Kraken::KrakenClient exchange;
    std::unique_ptr<IClock> clock;
    std::string pair;
    double capital;
};
