#pragma once
#include <string>
#include <nlohmann/json.hpp>


namespace Kraken {
    class KrakenTrade {
    public:
        friend void to_json(const nlohmann::json& j, KrakenTrade& trade) {
            throw std::logic_error("to_json is not yet implemented");
        }

        friend void from_json(const nlohmann::json& j, KrakenTrade& trade) {
            if (!j.is_array()) {
                throw std::runtime_error("KrakenTrade Json constructor must be a JSON array");
            }
            trade.price = std::stod(j.at(0).get<std::string>());
            trade.volume = std::stod(j.at(1).get<std::string>());
            trade.time = j.at(2).get<double>();
            trade.side = j.at(3).get<std::string>();
            trade.type = j.at(4).get<std::string>();
            trade.misc = j.at(5).get<std::string>();
            trade.tradeId = j.at(6).get<long>();
        }
        double getPrice() const { return price; }
        double getVolume() const { return volume; }
        double getTime() const { return time; }
        std::string getSide() const { return side; }
        std::string getType() const { return type; }
        std::string getMisc() const { return misc; }
        long getTradeId() const { return tradeId; }

        double setPrice(double _price) { price = _price; }
        double setVolume(double _volume) { volume = _volume; }
        double setTime(double _time) { time = _time; }
        std::string setSide(std::string _side) { side = _side; }
        std::string setType(std::string _type) { type = _type; }
        std::string setMisc(std::string _misc) { misc = _misc; }
        long setTradeId(long _tradeId) { tradeId = _tradeId; }

    private:
        double price;
        double volume;
        double time;
        std::string side;
        std::string type;
        std::string misc;
        long tradeId;
    };
}