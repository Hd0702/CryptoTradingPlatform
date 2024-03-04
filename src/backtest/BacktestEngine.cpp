#include "BacktestEngine.hpp"

void BacktestEngine::runBacktest(
    const std::unique_ptr<BaseStrategy> strategy,
    const std::string pair,
    const std::chrono::time_point<std::chrono::system_clock> start,
    const std::chrono::time_point<std::chrono::system_clock> end,
    const double capital
) {
    // iterate through each hour and run the strategy
    // remember, we're not testing the strategy is implemented correctly, just that it runs

    // we need to add some injections into our strategy. Inject in what the current time is and the trades
    for (auto time = start; time < end; time += std::chrono::hours(1)) {
        strategy->buy();
        strategy->sell();
    }
}