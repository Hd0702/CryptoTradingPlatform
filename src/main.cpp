#include <iostream>
#include "exchanges/kraken/KrakenClient.hpp"
#include "exchanges/coinbase/CoinbaseClient.hpp"

int main() {
    auto env_reader = Env::EnvReader("../.env");
    curl_global_init(CURL_GLOBAL_DEFAULT);
    Kraken::KrakenClient kclient = Kraken::KrakenClient(env_reader);
    Coinbase::CoinbaseClient cclient = Coinbase::CoinbaseClient(env_reader, Clock());
    std::string response;
    do {
        response = cclient.ListAccounts();
        std::cout << response << std::endl;
    } while (response.find("Unauthorized") == std::string::npos);
    curl_global_cleanup();
    return 0;
}
