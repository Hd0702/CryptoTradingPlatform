#pragma once

#include "MovingAverageTrade.hpp"
#include "exchanges/kraken/KrakenClient.hpp"
#include "exchanges/kraken/KrakenLoader.hpp"
#include "strategy/BaseStrategy.hpp"

namespace Kraken {
    class MovingAverageCrossover final : public BaseStrategy {
    public:
        explicit MovingAverageCrossover(
            const KrakenClient& exchange,
            std::unique_ptr<IClock> clock,
            const bool dryRun = false
            ): BaseStrategy(exchange, KrakenLoader(exchange), std::move(clock), dryRun) {};
        void buy() override;
        void sell() override;
        [[nodiscard]] std::vector<MovingAverageTrade> loadInFlightTrades();

        [[nodiscard]] bool check(const std::pair<int, int>&trade, const std::string& pair, bool buy = false) const;
        void buyOrSell();
    private:
        [[nodiscard]] double getVwap(const std::vector<KrakenOHLC>& ohlcPoints) const;
    };
}
