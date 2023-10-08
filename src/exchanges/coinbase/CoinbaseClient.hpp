#pragma once

#include <string>
#include <vector>
#include <curl/curl.h>

#include "config/EnvReader.hpp"
#include "time/Clock.hpp"

namespace Coinbase {

    class CoinbaseClient {
    public:
        CoinbaseClient(const Env::EnvReader& env_reader_instance, Clock clock);
        ~CoinbaseClient();
        std::string listAccounts();
    private:
        const std::string url = "https://api.coinbase.com";
        std::string
        generateSignature(const std::string &method, const std::string &requestPath, const std::string &body,
                          const std::string &timestamp);
        Clock clock;
        CURL* curl;
        static size_t curlCallback(char* ptr, size_t size, size_t nmemb, void* userdata);
        std::string key;
        std::string secret;
    };

}

