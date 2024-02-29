#pragma once
#include <string>
#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json.hpp>

struct MovingAverageTrade {
    // id of the original order placed
    std::string marketOrderId;
    // id of the limit order placed
    std::string limitOrderId;
    // time period of the first window
    long firstWindow;
    // time period of the second window
    long secondWindow;
    // name of the asset pair
    std::string pair;
    // amount of the asset pair
    double amount;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MovingAverageTrade, marketOrderId, limitOrderId, firstWindow, secondWindow, pair)
};
