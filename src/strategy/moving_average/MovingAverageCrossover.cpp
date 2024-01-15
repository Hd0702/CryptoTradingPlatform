#include "MovingAverageCrossover.hpp"
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

namespace Kraken {
    MovingAverageCrossover::MovingAverageCrossover(std::shared_ptr<BaseExchange> ex): BaseStrategy(ex) {}

    bool MovingAverageCrossover::buy() {
        throw std::logic_error("Function not implemented yet");
    }

    bool MovingAverageCrossover::sell() {
        throw std::logic_error("Function not implemented yet");
    }

    std::vector<MovingAverageCrossover::MovingAverageTrade> MovingAverageCrossover::loadInFlightTrades() {
        // first thing is we load in all transactions to see how much of that asset we have that wasn't sold
        // The end of this we should know how much of the asset we have. Then we can call check.
        // std::filesystem::path path = "trades/kraken/moving_average.json";
        // std::vector<MovingAverageCrossover::MovingAverageTrade> trades;
        // if (std::filesystem::exists(path)) {
        //     std::ifstream file(path);
        //     nlohmann::json j;
        //     file >> j;
        //     for (auto iter = j.begin(); iter != j.end(); ++iter) {
        //         MovingAverageTrade obj;
        //         from_json(*iter, obj); // Convert JSON array to CustomObject
        //         trades.push_back(obj);
        //     }
        // }
        // 1. Check each order in the list to see if the limit order was filled. If not then we have the opportunity to fill it ourselves
        // 2. for each order, check back how ever many time points for the two windows. This should return what the current moving average is.
        // 3. still iteratiing over each order, check if the sell moving average was hit.

        // buying workflow
        // 1. Iterate through a set of pre-determined moving averages for now.
        // 2. calculate the OHLC from the last few data points to see if a buy it hit.

        // How to manage capital
        // For now I think it's okay to assume each trade is $20 USD to start.

    }

    void MovingAverageCrossover::buyOrSell() {
        // knowing how much of an asset we have we can check if we can buy or sell
        // Then we check the window size (hr, minutes, days, etc) and see if we currently want to sell because we crossed over in the last hour or so and save the transaction to the file.
    }
}
