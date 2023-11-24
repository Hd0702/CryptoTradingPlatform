#include "KrakenOHLC.hpp"
namespace Kraken {
    void to_json(nlohmann::json& j, KrakenOHLC& ohlc) {
        j = nlohmann::json::array({ohlc.time, ohlc.open, ohlc.high, ohlc.low, ohlc.close, ohlc.vwap, ohlc.volume, ohlc.count});
    }

    void from_json(const nlohmann::json& j, KrakenOHLC& ohlc) {
        if (!j.is_array()) {
            throw std::runtime_error("KrakenOHLC Json constructor must be a JSON array");
        }
        ohlc.time = std::stol(j.at(1).get<std::string>());
        ohlc.open = std::stod(j.at(2).get<std::string>());
        ohlc.high = std::stod(j.at(3).get<std::string>());
        ohlc.low = std::stod(j.at(4).get<std::string>());
        ohlc.close = std::stod(j.at(5).get<std::string>());
        ohlc.vwap = std::stod(j.at(6).get<std::string>());
        ohlc.volume = std::stod(j.at(7).get<std::string>());
        ohlc.count = std::stol(j.at(8).get<std::string>());
    }
}