//
// Created by Hayden Donofrio on 2/23/23.
//

#pragma once
#include <string>
#include <curl/curl.h>

#include "../../config/EnvReader.hpp"

namespace Kraken {
    class KrakenClient {
    public:
        static constexpr std::string_view url = "https://api.kraken.com";
        KrakenClient(Env::EnvReader& env_reader_instance);
        std::string GetServerTime();
        std::string GetBalance();
    private:
        static constexpr std::string_view timeURL = "/0/public/Time";
        void Init();
        const std::string GenerateNonce();
        std::string key;
        std::string secret;
        CURL* curl;
        Env::EnvReader& env_reader;
        static size_t CurlCallback(char* ptr, size_t size, size_t nmemb, void* userdata);
        [[nodiscard]] std::string Signature(const std::string &path, const std::string &nonce, const std::string &postData) const;
    };
}
