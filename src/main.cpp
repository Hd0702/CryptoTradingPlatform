#include <iostream>
#include "exchanges/kraken/KrakenClient.hpp"
#include "exchanges/kraken/KrakenLoader.hpp"
#include "exchanges/coinbase/CoinbaseClient.hpp"

int main() {
    auto env_reader = Env::EnvReader(".env");
    curl_global_init(CURL_GLOBAL_DEFAULT);
    Kraken::KrakenClient kclient = Kraken::KrakenClient(env_reader);
    Coinbase::CoinbaseClient cclient = Coinbase::CoinbaseClient(env_reader, Clock());
    Kraken::KrakenLoader loader(kclient, "exchange_files");
    kclient.getOHLC(1559350785297011117L, "XETHZUSD");
    loader.fetchData(1700488979LL, "XETHZUSD");
//    std::string response = "sfasf";
//    auto now = std::chrono::system_clock::now();
//    const auto oneDayAgo = now - std::chrono::hours(24);
//    const long long oneDayAgoMillis = std::chrono::duration_cast<std::chrono::milliseconds>(oneDayAgo.time_since_epoch()).count();
//    // hardcoded values for testing purposes replace later
//    //response = kclient.buy("ETHUSDT", "0.01", "sell", "market");
//    auto result = kclient.getOHLC(1559350785297011117L, "XETHZUSD");
//    auto elapsed = std::chrono::system_clock::now() - now;
//    long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
//    std::cout << response << microseconds << std::endl;
//    return 0;
}
