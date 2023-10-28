#include "OHLC.hpp"
namespace Kraken {
    void to_json(const nlohmann::json& j, OHLC& ohlc) {
        throw std::logic_error("to_json is not yet implemented");
    }

    void from_json(const nlohmann::json& j, OHLC& ohlc) {
        if (!j.is_array()) {
            throw std::runtime_error("OHLC Json constructor must be a JSON array");
        }
        ohlc.time = std::stol(j.at(1).get<std::string>());
        ohlc.open = std::stod(j.at(1).get<std::string>());
        ohlc.high = std::stod(j.at(1).get<std::string>());
        ohlc.low = std::stod(j.at(1).get<std::string>());
        ohlc.close = std::stod(j.at(1).get<std::string>());
        ohlc.vwap = std::stod(j.at(1).get<std::string>());
        ohlc.volume = std::stod(j.at(1).get<std::string>());
        ohlc.count = std::stol(j.at(1).get<std::string>());
    }
}