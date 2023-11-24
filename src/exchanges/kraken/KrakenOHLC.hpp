#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include "../BaseOHLC.hpp"

namespace Kraken {
    class KrakenOHLC final : public BaseOHLC {
    public:
        friend void to_json( nlohmann::json& j, KrakenOHLC& ohlc);
        friend void from_json(const nlohmann::json& j, KrakenOHLC& ohlc);
        long getTime() const { return time; }
        double getOpen() const { return open; }
        double getHigh() const { return high; }
        double getLow() const { return low; }
        double getClose() const { return close; }
        double getVwap() const { return vwap; }
        double getVolume() const { return volume; }
        long getCount() const { return count; }

        void setTime(long _time) { time = _time; }
        void setOpen(double _open) { open = _open; }
        void setHigh(double _high) { high = _high; }
        void setLow(double _low) { low = _low; }
        void setClose(double _close) { close = _close; }
        void setVwap(double _vwap) { vwap = _vwap; }
        void setVolume(double _volume) { volume = _volume; }
        void setCount(long _count) { count = _count; }
    private:
        double vwap;
        double volume;
        long count;
    };
}