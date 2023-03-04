#include <iostream>
#include "exchanges/KrakenClient.h"

int main() {
    auto env_reader = Env::EnvReader("../.env");
    Kraken::KrakenClient kclient = Kraken::KrakenClient(env_reader);
    std::cout << "Hello, World from Kraken: " << kclient.GetBalance() << std::endl;
    return 0;
}
