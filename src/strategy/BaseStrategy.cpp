#include "BaseStrategy.hpp"
#include <iostream>

BaseStrategy::BaseStrategy(const Kraken::KrakenClient& exchange_, const Kraken::KrakenLoader loader_, std::unique_ptr<IClock> clock_, bool dryRun_):
    exchange(exchange_), loader(loader_), clock(std::move(clock_)), dryRun(dryRun_) {}

Kraken::KrakenOrder BaseStrategy::makeOrder(const std::string& volume, const std::string& type, const std::string& orderType) const {
    auto order = exchange.buy(pair, volume, type, orderType, dryRun);
    if (!dryRun) {
        return order;
    }
    // get price from files and add to order
    // once that is done edit the strategy code to use this method when making order.
    // Then we can continue with the backtesting code
    return order;
}