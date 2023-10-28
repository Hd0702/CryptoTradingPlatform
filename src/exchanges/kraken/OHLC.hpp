#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace Kraken {
    class OHLC {
    public:
        friend void to_json(const nlohmann::json& j, OHLC& ohlc);
        friend void from_json(const nlohmann::json& j, OHLC& ohlc);
    private:
        long time;
        double open;
        double high;
        double low;
        double close;
        double vwap;
        double volume;
        long count;
    };
}