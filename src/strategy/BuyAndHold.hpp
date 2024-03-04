#pragma once
#include "BaseStrategy.hpp"

class BuyAndHold final: BaseStrategy {
    // buy and hold is special since it has no sell point, as it is mostly use for baseline purposes.
    // To keep things simple across all strategies we will assume the sell point is randomly 365 days after the buy point.
public:
    explicit BuyAndHold(const Kraken::KrakenClient& exchange);
    virtual void buy() override;
    virtual void sell() override;

};


