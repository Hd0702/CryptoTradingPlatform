#pragma once
#include <string>
#include "exchanges/kraken/KrakenClient.hpp"
#include "exchanges/kraken/KrakenLoader.hpp"
#include "time/IClock.hpp"

class BaseStrategy {
public:
    virtual void buy() = 0;
    virtual void sell() = 0;
    // Todo: add a way to load in strategy history and if any triggers were hit since the last trade.
protected:
    // Should add a filepath for current trades
    explicit BaseStrategy(const Kraken::KrakenClient& exchange_, Kraken::KrakenLoader loader,std::unique_ptr<IClock> clock_, bool dryRun_ = false);
    [[nodiscard]] Kraken::KrakenOrder makeOrder(const std::string& volume, const std::string& type, const std::string& orderType) const;
    const Kraken::KrakenClient exchange;
    const Kraken::KrakenLoader loader;
    bool dryRun = false;
    std::unique_ptr<IClock> clock;
    std::string pair;
    double capital;
};
