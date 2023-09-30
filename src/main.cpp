#include <iostream>
#include "exchanges/kraken/KrakenClient.hpp"
#include "exchanges/coinbase/CoinbaseClient.hpp"
#include <json/json.h>

int main() {
    auto env_reader = Env::EnvReader("../.env");
    curl_global_init(CURL_GLOBAL_DEFAULT);
    Kraken::KrakenClient kclient = Kraken::KrakenClient(env_reader);
    Coinbase::CoinbaseClient cclient = Coinbase::CoinbaseClient(env_reader, Clock());
    std::string response;
    auto now = std::chrono::system_clock::now();
    const auto oneDayAgo = now - std::chrono::hours(24);
    const long long oneDayAgoMillis = std::chrono::duration_cast<std::chrono::milliseconds>(oneDayAgo.time_since_epoch()).count();
    response = kclient.buy();
    Json::Value root;
    auto elapsed = std::chrono::system_clock::now() - now;
    long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    std::cout << response << microseconds << std::endl;
    return 0;
}
