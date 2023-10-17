#pragma once
#include "ActionPoint.hpp"
#include <optional>

class BaseStrategy {
private:
    ActionPoint buyAction;
    ActionPoint sellAction;
    std::optional<ActionPoint> limitAction;
    // needs a buy point and a sell point, commodoity and exchange
    // for knowing when to buy we need a change over time.
    // like percent change over a time delta
};