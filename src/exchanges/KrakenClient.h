//
// Created by Hayden Donofrio on 2/23/23.
//

#ifndef CRYPTOTRADINGPLATFORM_KRAKENCLIENT_H
#define CRYPTOTRADINGPLATFORM_KRAKENCLIENT_H

#include <string>
#include <curl/curl.h>

#include "../config/EnvReader.h"

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
        static std::vector<unsigned char> B64Decode(const std::string& data);

        const unsigned char *  HMACSha512(const std::vector<unsigned char> &data, const std::vector<unsigned char> &key) const;

        const std::string Signature(const std::string &path, const std::string &nonce, const std::string &postData) const;

        const std::string B64Encode(const unsigned char* data) const;
    };
}

#endif //CRYPTOTRADINGPLATFORM_KRAKENCLIENT_H
