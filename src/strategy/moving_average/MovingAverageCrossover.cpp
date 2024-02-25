#include "MovingAverageCrossover.hpp"
#include <fstream>
#include <ranges>

namespace Kraken {
    // We should just verify these with backtesting or even better to make it dynamic one day
    static constexpr std::array<std::pair<long, long>, 3> windows = {
        std::pair{ 2, 5 },
        std::pair{ 12, 20 },
        std::pair{ 20, 50 }
    };

    MovingAverageCrossover::MovingAverageCrossover(const KrakenClient& exchange): exchange(exchange), loader(KrakenLoader(exchange)) {}

    // NEXT STEP IS MAKING THE BUY METHOD RETURN AN OBJECT INSTEAD OF A RAW STRING THEN WE CAN DO THIS AUTOMATED BUY LOGIC
    void MovingAverageCrossover::buy() {
        // wrap windows around an std::views
        const auto trades = windows | std::views::filter([&](const auto& window) {
            return check(window, "XETHZUSD", true);
        }) | std::views::transform([&](const auto& window) {
            // start with dummy value
            MovingAverageTrade tradeInfo;
            tradeInfo.firstWindow = window.first;
            tradeInfo.secondWindow = window.second;
            tradeInfo.pair = "XETHZUSD";
            tradeInfo.amount = 0.01;
            // TODO: make the buy client call wrapped in a json object
            return tradeInfo;
        });
    }

    void MovingAverageCrossover::sell() {
        const auto inFlightTrades = loadInFlightTrades();
        const auto tradeToSell = inFlightTrades | std::views::transform([&](const MovingAverageTrade& trade) {
            return check({trade.firstWindow, trade.secondWindow}, trade.pair, false);
        });
        assert(tradeToSell.size() == inFlightTrades.size() && "Trade to sell size does not match in flight trades size");
        const auto sequence = std::views::iota(0, static_cast<int>(inFlightTrades.size()))
        | std::views::filter([&](int index) { return tradeToSell[index]; })
        | std::views::transform([&](int index) { return inFlightTrades[index]; })
        | std::ranges::to<std::vector<MovingAverageTrade>>();
        // itereate over the sequence
        for (const auto& trade : sequence) {
            std::cout << "Selling " << trade.amount << " of " << trade.pair << " at market price" << std::endl;
            exchange.buy(trade.pair, std::to_string(trade.amount), "sell", "market");
            // we should probably delete the old file entry here
        }
    }

    std::vector<MovingAverageTrade> MovingAverageCrossover::loadInFlightTrades() {
        // load in existing trades from the exchange
        // TODO: move this to a folder so we dont have to read the whole thing all of the time.
        const nlohmann::json j = nlohmann::json::parse(std::ifstream("exchange_files/kraken/trades/moving_average_trades.json"));
        auto trades = j.get<std::vector<MovingAverageTrade>>();
        // check to see if limits have closed
        const std::vector<std::string> vec = trades | std::views::transform([&](const MovingAverageTrade& trade) { return std::to_string(trade.limitOrderId); }) | std::ranges::to<std::vector<std::string>>();
        const auto orders = exchange.getTradeInfo(vec);
        const auto limitOrders = exchange.getTradeInfo(
            trades | std::views::transform([&](const MovingAverageTrade& trade) { return std::to_string(trade.limitOrderId); })
            | std::ranges::to<std::vector<std::string>>()
        );
        auto openTrades = trades | std::views::filter([&](const MovingAverageTrade& trade) {
            const auto limitOrderString = std::to_string(trade.limitOrderId);
            return limitOrders.contains(limitOrderString) && limitOrders.at(limitOrderString).posstatus == "open";
        });
        return {openTrades.begin(), openTrades.end()};
    }

    // only exists for selling purpopses
    bool MovingAverageCrossover::check(const std::pair<int, int>& trade, const std::string& pair, const bool buy) const {
        // now that we have the trades that are open we need to load in the previous x amount of hours for each window.
        // check if they crossed over in the last hour. If so we sell or buy depending on the direction of the crossover.
            const auto& [firstWindow, secondWindow] = trade;
            const auto largerWindow = std::max(firstWindow, secondWindow);
            const auto currentHour = std::chrono::floor<std::chrono::hours>(std::chrono::system_clock::now().time_since_epoch());
            const auto windowRange = std::views::iota(0, largerWindow + 1);
            const auto prices = windowRange | std::views::transform([&](int i) {
                return getVwap(loader.fetchData(currentHour - std::chrono::hours(i), pair));
            });
            // just need two values for each window
            std::vector<double> window1Values = { 0 };
            std::vector<double> window2Values = { 0 };
            for (int i =0; i < prices.size(); ++i) {
                if (i <= firstWindow) {
                    window1Values[0] += prices[i];
                    if (i == firstWindow) window1Values[0] /= firstWindow;
                } else {
                    window1Values.push_back((window1Values.back() + prices[i]) / firstWindow);
                }
                // repeat for the smaller window
                if (i <= secondWindow) {
                    window2Values[0] += prices[i];
                    if (i == secondWindow) window2Values[0] /= secondWindow;
                } else {
                    window2Values.push_back((window2Values.back() + prices[i]) / secondWindow);
                }
            }

            if (buy) {
                // shorter average goes above longer average we buy
                if (firstWindow < secondWindow) {
                    return window1Values.back() > window2Values.back() && window1Values[window1Values.size() - 2] < window2Values[window2Values.size() - 2];
                }
                return window2Values.back() > window1Values.back() && window2Values[window1Values.size() - 2] < window1Values[window2Values.size() - 2];
            }
            // sell if the larger window goes below the smaller window
            if (firstWindow < secondWindow) {
                return window1Values.back() < window2Values.back() && window1Values[window1Values.size() - 2] > window2Values[window2Values.size() - 2];
            }
            return window2Values.back() < window1Values.back() && window2Values[window1Values.size() - 2] > window1Values[window2Values.size() - 2];
    }

    double MovingAverageCrossover::getVwap(const std::vector<KrakenOHLC>& ohlcPoints) const {
        const auto [price, volume] = std::reduce(
            ohlcPoints.cbegin(),
            ohlcPoints.cend(),
            std::array{0.0, 0.0}, [](std::array<double, 2> priceAndVolume, const KrakenOHLC& ohlc) -> std::array<double, 2> {
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
