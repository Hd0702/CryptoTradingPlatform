#pragma once
#include "BaseStrategy.hpp"

class BuyAndHold final: BaseStrategy {
    // buy and hold is special since it has no sell point, as it is mostly use for baseline purposes.
    // To keep things simple across all strategies we will assume the sell point is randomly 365 days after the buy point.
public:
    explicit BuyAndHold(std::shared_ptr<BaseExchange> exchange);
    virtual bool buy() override;
    virtual bool sell() override;

};


