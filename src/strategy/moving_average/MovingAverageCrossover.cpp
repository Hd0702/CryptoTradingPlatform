#include "MovingAverageCrossover.hpp"
#include <fstream>
#include <ranges>

namespace Kraken {
    MovingAverageCrossover::MovingAverageCrossover(const KrakenClient& exchange): exchange(exchange), loader(KrakenLoader(exchange)) {}

    bool MovingAverageCrossover::buy() {
        throw std::logic_error("Function not implemented yet");
    }

    bool MovingAverageCrossover::sell() {
        throw std::logic_error("Function not implemented yet");
    }

    std::vector<MovingAverageTrade> MovingAverageCrossover::loadInFlightTrades() {
        // load in existing trades from the exchange
        const nlohmann::json j = nlohmann::json::parse(std::ifstream("exchange_files/kraken/trades"));
        auto trades = j.get<std::vector<MovingAverageTrade>>();
        // check to see if limits have closed
        const auto limitOrders = exchange.getTradeInfo(
            std::transform_reduce(trades.cbegin(), trades.cend(), std::vector<std::string>(), [](std::vector<std::string>& accum, const MovingAverageTrade& trade) {
                accum.push_back(std::to_string(trade.limitOrderId));
                return accum;
            }, std::plus()));
        auto openTrades = trades | std::views::filter([&](const MovingAverageTrade& trade) {
            const auto limitOrderString = std::to_string(trade.limitOrderId);
            return limitOrders.contains(limitOrderString) && limitOrders.at(limitOrderString).posstatus == "open";
        });
        return {openTrades.begin(), openTrades.end()};
    }

    void MovingAverageCrossover::check(std::vector<MovingAverageTrade> trades) {
        // now that we have the trades that are open we need to load in the previous x amount of hours for each window.
        // check if they crossed over in the last hour. If so we sell or buy depending on the direction of the crossover.
    }

    void MovingAverageCrossover::buyOrSell() {
        // knowing how much of an asset we have we can check if we can buy or sell
        // Then we check the window size (hr, minutes, days, etc) and see if we currently want to sell because we crossed over in the last hour or so and save the transaction to the file.
    }
}
