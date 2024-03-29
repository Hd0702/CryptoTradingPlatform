#include "MovingAverageCrossover.hpp"
#include <fstream>
#include <ranges>
#include <algorithm>


namespace Kraken {
    // We should just verify these with backtesting or even better to make it dynamic one day
    static constexpr std::array<std::pair<long, long>, 3> windows = {
        std::pair{2, 5},
        std::pair{12, 20},
        std::pair{20, 50}
    };

    void MovingAverageCrossover::buy() {
        // TODO: Check capital here
        // wrap windows around an std::views
        const auto newTrades = windows | std::views::filter([&](const auto&window) {
            return check(window, "XETHZUSD", true);
        }) | std::views::transform([&](const auto&window) {
            // start with dummy value
            MovingAverageTrade tradeInfo;
            tradeInfo.firstWindow = window.first;
            tradeInfo.secondWindow = window.second;
            tradeInfo.pair = "XETHZUSD";
            tradeInfo.amount = 0.01;
            return tradeInfo;
        }) | std::views::take_while([&](const MovingAverageTrade&trade) {
            if (trade.amount > capital) return false;
            capital -= trade.amount;
            return true;
        }) | std::views::transform([&](MovingAverageTrade tradeInfo) {
            const KrakenOrder buyOrder = exchange.buy(tradeInfo.pair, std::to_string(tradeInfo.amount), "buy", "market", dryRun);
            if (buyOrder.txid.size() > 1) {
                throw std::invalid_argument(std::format("We should only have one buy order but we have {}",
                                                        buyOrder.txid.size()));
            }
            tradeInfo.marketOrderId = buyOrder.txid[0];
            const auto sellOrder = exchange.buy(tradeInfo.pair, std::to_string(tradeInfo.amount), "sell", "limit", dryRun);
            if (sellOrder.txid.size() > 1) {
                throw std::invalid_argument(std::format("We should only have one sell order but we have {}",
                                                        sellOrder.txid.size()));
            }
            tradeInfo.limitOrderId = sellOrder.txid[0];
            return tradeInfo;
        }) | std::ranges::to<std::vector<MovingAverageTrade>>();
        std::ranges::for_each(newTrades, [&](const MovingAverageTrade&trade) {
            std::ofstream file("exchange_files/kraken/trades/moving_average/" + trade.marketOrderId);
            file << nlohmann::json(trade);
        });
    }

    void MovingAverageCrossover::sell() {
        const auto inFlightTrades = loadInFlightTrades();
        const auto tradeToSell = inFlightTrades | std::views::transform([&](const MovingAverageTrade&trade) {
            return check({trade.firstWindow, trade.secondWindow}, trade.pair, false);
        });
        assert(
            tradeToSell.size() == inFlightTrades.size() && "Trade to sell size does not match in flight trades size");
        const std::vector<MovingAverageTrade> sequence = std::views::iota(0, static_cast<int>(inFlightTrades.size()))
                              | std::views::filter([&](int index) { return tradeToSell[index]; })
                              | std::views::transform([&](int index) { return inFlightTrades[index]; })
                              | std::ranges::to<std::vector<MovingAverageTrade>>();
        // itereate over the sequence
        for (const MovingAverageTrade&trade: sequence) {
            std::cout << "Selling " << trade.amount << " of " << trade.pair << " at market price" << std::endl;
            const auto sellOrder = exchange.buy(trade.pair, std::to_string(trade.amount), "sell", "market", dryRun);
            capital += trade.amount;
            const auto cancelOrder = exchange.cancelOrder(trade.limitOrderId);
            std::ofstream file("exchange_files/kraken/trades/moving_average/_complete/" + trade.marketOrderId);
            file << nlohmann::json(trade);
            std::filesystem::remove("exchange_files/kraken/trades/moving_average/" + trade.marketOrderId);
        }
    }

    std::vector<MovingAverageTrade> MovingAverageCrossover::loadInFlightTrades() {
        // load in existing trades from the exchange
        // TODO: move this to a folder so we dont have to read the whole thing all of the time.
        std::vector<MovingAverageTrade> trades;
        for (const auto&file: std::filesystem::directory_iterator("exchange_files/kraken/trades/moving_average/")) {
            const nlohmann::json j = nlohmann::json::parse(std::ifstream(file.path()));
            trades.push_back(j.get<MovingAverageTrade>());
        }
        // check to see if limits have closed
        const std::vector<std::string> vec = trades
                                             | std::views::transform([&](const MovingAverageTrade&trade) {
                                                 return trade.limitOrderId;
                                             })
                                             | std::ranges::to<std::vector<std::string>>();
        const auto orders = exchange.getTradeInfo(vec);
        // we need to add capital and remove trades that hit a limit order.
        const auto limitOrders = exchange.getTradeInfo(
            trades | std::views::transform([&](const MovingAverageTrade&trade) { return trade.limitOrderId; })
            | std::ranges::to<std::vector<std::string>>()
        );
        auto openTrades = trades | std::views::filter([&](const MovingAverageTrade&trade) {
            if (limitOrders.contains(trade.limitOrderId)) {
                if (limitOrders.at(trade.limitOrderId).posstatus == "open") return true;
                std::ofstream file("exchange_files/kraken/trades/moving_average/_complete/" + trade.marketOrderId);
                file << nlohmann::json(trade);
                std::filesystem::remove("exchange_files/kraken/trades/moving_average/" + trade.marketOrderId);
                capital += trade.amount;
            }
            return false;
        });
        return {openTrades.begin(), openTrades.end()};
    }

    // only exists for selling purpopses
    bool MovingAverageCrossover::check(const std::pair<int, int>&trade, const std::string&pair, const bool buy) const {
        // now that we have the trades that are open we need to load in the previous x amount of hours for each window.
        // check if they crossed over in the last hour. If so we sell or buy depending on the direction of the crossover.
        const auto&[firstWindow, secondWindow] = trade;
        const auto largerWindow = std::max(firstWindow, secondWindow);
        // inject some time into this current hour plz
        const auto currentHour = std::chrono::floor<std::chrono::hours>(clock->now().time_since_epoch());
        const auto windowRange = std::views::iota(0, largerWindow + 1);
        const auto prices = windowRange | std::views::transform([&](int i) {
            return getVwap(loader.fetchData(currentHour - std::chrono::hours(i), pair));
        });
        // just need two values for each window
        std::vector<double> window1Values = {0};
        std::vector<double> window2Values = {0};
        for (int i = 0; i < prices.size(); ++i) {
            if (i <= firstWindow) {
                window1Values[0] += prices[i];
                if (i == firstWindow) window1Values[0] /= firstWindow;
            }
            else {
                window1Values.push_back((window1Values.back() + prices[i]) / firstWindow);
            }
            // repeat for the smaller window
            if (i <= secondWindow) {
                window2Values[0] += prices[i];
                if (i == secondWindow) window2Values[0] /= secondWindow;
            }
            else {
                window2Values.push_back((window2Values.back() + prices[i]) / secondWindow);
            }
        }

        if (buy) {
            // shorter average goes above longer average we buy
            if (firstWindow < secondWindow) {
                return window1Values.back() > window2Values.back() && window1Values[window1Values.size() - 2] <
                       window2Values[window2Values.size() - 2];
            }
            return window2Values.back() > window1Values.back() && window2Values[window1Values.size() - 2] <
                   window1Values[window2Values.size() - 2];
        }
        // sell if the larger window goes below the smaller window
        if (firstWindow < secondWindow) {
            return window1Values.back() < window2Values.back() && window1Values[window1Values.size() - 2] >
                   window2Values[window2Values.size() - 2];
        }
        return window2Values.back() < window1Values.back() && window2Values[window1Values.size() - 2] > window1Values[
                   window2Values.size() - 2];
    }

    double MovingAverageCrossover::getVwap(const std::vector<KrakenOHLC>&ohlcPoints) const {
        const auto [price, volume] = std::reduce(
            ohlcPoints.cbegin(),
            ohlcPoints.cend(),
            std::array{0.0, 0.0},
            [](std::array<double, 2> priceAndVolume, const KrakenOHLC&ohlc) -> std::array<double, 2> {
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
