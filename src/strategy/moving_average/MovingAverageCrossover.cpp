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
            std::reduce(trades.cbegin(), trades.cend(), std::vector<std::string>(), [](std::vector<std::string>& accum, const MovingAverageTrade& trade) {
                accum.push_back(std::to_string(trade.limitOrderId));
                return accum;
            }));
        auto openTrades = trades | std::views::filter([&](const MovingAverageTrade& trade) {
            const auto limitOrderString = std::to_string(trade.limitOrderId);
            return limitOrders.contains(limitOrderString) && limitOrders.at(limitOrderString).posstatus == "open";
        });
        return {openTrades.begin(), openTrades.end()};
    }

    // only exists for selling purpopses
    bool MovingAverageCrossover::check(std::vector<MovingAverageTrade> trades) {
        // now that we have the trades that are open we need to load in the previous x amount of hours for each window.
        // check if they crossed over in the last hour. If so we sell or buy depending on the direction of the crossover.
        std::ranges::for_each(trades, [&](const MovingAverageTrade& trade) {
            const auto largerWindow = std::max(trade.firstWindow, trade.secondWindow);
            const auto currentHour = std::chrono::floor<std::chrono::hours>(std::chrono::system_clock::now().time_since_epoch());
            const auto windowRange = std::views::iota(0, largerWindow + 1);
            const auto prices = windowRange | std::views::transform([&](int i) {
                return getVwap(loader.fetchData(currentHour - std::chrono::hours(i), trade.pair));
            });
            // just need two values for each window
            std::vector<double> window1Values = { 0 };
            std::vector<double> window2Values = { 0 };
            for (int i =0; i < prices.size(); ++i) {
                if (i <= trade.firstWindow) {
                    window1Values[0] += prices[i];
                    if (i == trade.firstWindow) window1Values[0] /= trade.firstWindow;
                } else {
                    window1Values.push_back((window1Values.back() + prices[i]) / trade.firstWindow);
                }
                // repeat for the smaller window
                if (i <= trade.secondWindow) {
                    window2Values[0] += prices[i];
                    if (i == trade.secondWindow) window2Values[0] /= trade.secondWindow;
                } else {
                    window2Values.push_back((window2Values.back() + prices[i]) / trade.secondWindow);
                }
            }
            // sell if the larger window goes below the smaller window
            if (trade.firstWindow > trade.secondWindow) {
                return window1Values.back() < window2Values.back() && window1Values[window1Values.size() - 2] > window2Values[window2Values.size() - 2];
            }
            return window1Values.back() > window2Values.back() && window1Values[window1Values.size() - 2] < window2Values[window2Values.size() - 2];
        });
    }

    double MovingAverageCrossover::getVwap(std::vector<KrakenOHLC> ohlcPoints) const {
        const auto [price, volume] = std::reduce(ohlcPoints.cbegin(), ohlcPoints.cend(), std::array{0.0, 0.0}, [](std::array<double, 2> priceAndVolume, const KrakenOHLC& ohlc) {
            // vwap is acting as price since we don't have individual trades here. Just data aggregated by minute
            return {priceAndVolume[0] + ohlc.getVwap(), priceAndVolume[1] + ohlc.getVolume()};
        });
        return price / volume;
    }

    void MovingAverageCrossover::buyOrSell() {
        // knowing how much of an asset we have we can check if we can buy or sell
        // Then we check the window size (hr, minutes, days, etc) and see if we currently want to sell because we crossed over in the last hour or so and save the transaction to the file.
    }
}
