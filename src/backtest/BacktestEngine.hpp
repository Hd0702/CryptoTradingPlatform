#pragma once

#include "strategy/BaseStrategy.hpp"

class BacktestEngine {
public:
    /**
     * Simulates running a particular strategy and creates a CSV report of each trade and the profit from the trade
     * Runs buy and sell for every hour in the range of start and end
     */
    void runBacktest(
        const std::unique_ptr<BaseStrategy> strategy,
        const std::string pair,
        const std::chrono::time_point<std::chrono::system_clock> start,
        const std::chrono::time_point<std::chrono::system_clock> end,
        const double capital
    );
};
