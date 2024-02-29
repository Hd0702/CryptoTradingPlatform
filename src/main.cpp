#include <iostream>
#include "exchanges/kraken/KrakenClient.hpp"
#include "exchanges/kraken/KrakenLoader.hpp"
#include "exchanges/coinbase/CoinbaseClient.hpp"

int main() {
    auto env_reader = Env::EnvReader(".env");
    curl_global_init(CURL_GLOBAL_DEFAULT);
    Kraken::KrakenClient kclient = Kraken::KrakenClient(env_reader);
    Coinbase::CoinbaseClient cclient = Coinbase::CoinbaseClient(env_reader, Clock());
    auto items = kclient.getTradeInfo({"TG3PXU-WIYH3-ZJY44V", "TUBBK6-6CPHX-K4S3UO"});
    Kraken::KrakenLoader loader(kclient, "exchange_files");
    const auto currentHour = std::chrono::floor<std::chrono::hours>(std::chrono::system_clock::now().time_since_epoch());
    const auto  others = loader.fetchData(currentHour, "XETHZUSD");
    const auto order = kclient.buy("ETHUSDT", "0.01", "buy", "market", true);
    return 0;
}
