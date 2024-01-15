#pragma once
#include <string>
#include <optional>
struct MovingAverageTrade {
    long marketOrderId;
    std::optional<long> limitOrderId;
    long firstWindow;
    long secondWindow;
    std::string pair;
};
