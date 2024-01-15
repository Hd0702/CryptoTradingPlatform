#pragma once
#include "../BaseStrategy.hpp"
#include <string>

namespace Kraken {
    class MovingAverageCrossover final: public BaseStrategy {
        // each individual trade
        struct MovingAverageTrade {
            long marketOrderId;
            long limitOrderId;
            long firstWindow;
            long secondWindow;
            std::string pair;
        };
    /**
    Plan on how I get this to work.
    1. First thing is we need to load in all existing strategies that are in flight.
        - What does an in flight trade look like?
        - 1. we don't care about how much money is in a trade, just the current price of the asset and the criteria to sell
        - 2. this strategy will have info on window aggregation. We should check if we hit the window to sell in the last hour or two. If not just don't do anything
        - 3. We need to check if any of orders hit a stop loss. I think these are supported natively in kraken.
    2. How to know if we hit our marker or not? If we hit the two moving average windows in the last hour.
    3. What about backtesting?
        - We can make this work by accepting a timestamp in our check function. This is like a local override if needed, otherwise we just get the current OHLC and/or trades.
    **/
    public:
        explicit MovingAverageCrossover(std::shared_ptr<BaseExchange> exchange);
        virtual bool buy() override;
        virtual bool sell() override;
        std::vector<MovingAverageTrade> loadInFlightTrades();
        void buyOrSell();
    };
}