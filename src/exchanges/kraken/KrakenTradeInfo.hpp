#pragma once
#include <string>
class KrakenTradeInfo {
    std::string ordertxid;
    std::string postxid;
    std::string pair;
    long time;
    std::string type;
    std::string ordertype;
    std::string price;
    std::string cost;
    std::string fee;
    std::string vol;
    std::string margin;
    std::string leverage;
    std::string misc;
    long tradeId;
    std::optional<std::string> posstatus;
    std::optional<std::string> cprice;
    std::optional<std::string> ccost;
    std::optional<std::string> cfee;
    std::optional<std::string> cvol;
    std::optional<std::string> cmargin;
    std::optional<std::string> net;
    std::optional<std::string> trades;
public:
    // I wished nlohmann library supported optionals. *sigh*
    // once this is merged we should be good https://github.com/nlohmann/json/pull/2117/files
    friend void from_json(const nlohmann::json& j, KrakenTradeInfo& trade) {
        j.at("ordertxid").get_to(trade.ordertxid);
        j.at("postxid").get_to(trade.postxid);
        j.at("pair").get_to(trade.pair);
        j.at("time").get_to(trade.time);
        j.at("type").get_to(trade.type);
        j.at("ordertype").get_to(trade.ordertype);
        j.at("price").get_to(trade.price);
        j.at("cost").get_to(trade.cost);
        j.at("fee").get_to(trade.fee);
        j.at("vol").get_to(trade.vol);
        j.at("margin").get_to(trade.margin);
        j.at("leverage").get_to(trade.leverage);
        j.at("misc").get_to(trade.misc);
        j.at("trade_id").get_to(trade.tradeId);
        if (j.contains("posstatus")) {
            trade.posstatus = j.at("posstatus").get<std::string>();
        }
        if (j.contains("cprice")) {
            trade.cprice = j.at("cprice").get<std::string>();
        }
        if (j.contains("ccost")) {
            trade.ccost = j.at("ccost").get<std::string>();
        }
        if (j.contains("cfee")) {
            trade.cfee = j.at("cfee").get<std::string>();
        }
        if (j.contains("cvol")) {
            trade.cvol = j.at("cvol").get<std::string>();
        }
        if (j.contains("cmargin")) {
            trade.cmargin = j.at("cmargin").get<std::string>();
        }
        if (j.contains("net")) {
            trade.net = j.at("net").get<std::string>();
        }
        if (j.contains("trades")) {
            trade.trades = j.at("trades").get<std::string>();
        }
    }
 };