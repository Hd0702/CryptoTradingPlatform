#pragma once
#include <string>
#include <vector>
#include <curl/curl.h>

#include "../../config/EnvReader.hpp"
#include "OHLC.hpp"

namespace Kraken {
    class KrakenClient {
    public:
        static constexpr std::string_view url = "https://api.kraken.com";
        explicit KrakenClient(Env::EnvReader& env_reader_instance);
        ~KrakenClient();
        std::string getServerTime();
        std::string getBalance();
        std::vector<OHLC> getOHLC(const long long epochNanos, const std::string pair = "XETHZUSD") const;
        std::string buy(const std::string& pair, const std::string& volume, const std::string& type, const std::string& orderType) const;
    private:
        static constexpr std::string_view timeURL = "/0/public/Time";
        static constexpr std::string_view ohlcURL = "/0/public/OHLC";
        void Init();
        std::string nonce() const;
        std::string key;
        std::string secret;
        CURL* curl;
        Env::EnvReader& env_reader;
        static size_t curlCallback(char* ptr, size_t size, size_t nmemb, void* userdata);
        [[nodiscard]] std::string signature(const std::string &path, const std::string &nonce, const std::string &postData) const;
        std::string makePublicCall(const std::string& path, const std::unique_ptr<curl_slist> &headers=nullptr,const std::string& postdata = "") const;
        // TODO: try to refactor to unique_ptr
        using PostDataList = std::vector<const std::pair<std::string, std::string>>;
        std::string makePrivateCall(const std::string &pathSuffix, curl_slist *headers = nullptr, PostDataList postData = PostDataList()) const;

    };
}
