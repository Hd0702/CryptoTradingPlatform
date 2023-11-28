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
        [[nodiscard]] double getPrice() const { return price; }
        [[nodiscard]] double getVolume() const { return volume; }
        [[nodiscard]] double getTime() const { return time; }
        [[nodiscard]] std::string getSide() const { return side; }
        [[nodiscard]] std::string getType() const { return type; }
        [[nodiscard]] std::string getMisc() const { return misc; }
        [[nodiscard]] long getTradeId() const { return tradeId; }

        void setPrice(double _price) { price = _price; }
        void setVolume(double _volume) { volume = _volume; }
        void setTime(double _time) { time = _time; }
        void setSide(std::string _side) { side = _side; }
        void setType(std::string _type) { type = _type; }
        void voidsetMisc(std::string _misc) { misc = _misc; }
        void setTradeId(long _tradeId) { tradeId = _tradeId; }

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