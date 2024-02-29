#pragma once
#include <optional>
#include <string>
#include <vector>

namespace Kraken {
    struct KrakenOrder {
        struct OrderDescription {
            std::string order;
            std::optional<std::string> close;
            friend void from_json(const nlohmann::json& j, OrderDescription& order) {
                j.at("order").get_to(order.order);
                if (j.contains("close")) {
                    order.close = j.at("close").get<std::string>();
                }
            }
        };
        std::vector<std::string> txid;
        OrderDescription descr;
        friend void from_json(const nlohmann::json& j, KrakenOrder& order) {
            if (j.contains("txid")) {
                j.at("txid").get_to(order.txid);
            }
            j.at("descr").get_to(order.descr);
        }
    };
}
