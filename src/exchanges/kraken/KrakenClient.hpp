#pragma once
#include <string>
#include <curl/curl.h>

#include "../../config/EnvReader.hpp"

namespace Kraken {
    class KrakenClient {
    public:
        static constexpr std::string_view url = "https://api.kraken.com";
        explicit KrakenClient(Env::EnvReader& env_reader_instance);
        ~KrakenClient();
        std::string getServerTime();
        std::string getBalance();
        std::string getTradesSince(const long long epochMillis, const std::string pair = "ETHUSD") const;
        std::string buy() const;
    private:
        static constexpr std::string_view timeURL = "/0/public/Time";
        static constexpr std::string_view tradesURL = "/0/public/Trades";
        void Init();
        const std::string nonce() const;
        std::string key;
        std::string secret;
        CURL* curl;
        Env::EnvReader& env_reader;
        static size_t curlCallback(char* ptr, size_t size, size_t nmemb, void* userdata);
        [[nodiscard]] std::string signature(const std::string &path, const std::string &nonce, const std::string &postData) const;
        std::string makePublicCall(const std::string& path, const std::unique_ptr<curl_slist> &headers=nullptr,const std::string& postdata = "") const;
        // TODO: try to refactor to unique_ptr
        std::string makePrivateCall(const std::string &pathSuffix, curl_slist *headers) const;
    };
}
