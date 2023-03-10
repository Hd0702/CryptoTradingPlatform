
#include <boost/algorithm/string.hpp>
#include <openssl/buffer.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/bio.h>
#include <curl/curl.h>
#include <iostream>

#include "CoinbaseClient.hpp"
#include "encryption/EncryptionHelper.hpp"

namespace Coinbase {
    // Curl callbacks are not allowed to be C++ functions, so we need to use C functions.
    size_t CoinbaseClient::CurlCallback(char *ptr, size_t size, size_t nmemb, void *userdata) {
        auto response = static_cast<std::string *>(userdata);
        auto real_size = size * nmemb;

        response->append(ptr, real_size);
        return real_size;
    }

    CoinbaseClient::CoinbaseClient(const Env::EnvReader &env_reader_instance, Clock clock) : clock(clock) {
        key = env_reader_instance["COINBASE_API_KEY"];
        secret = env_reader_instance["COINBASE_API_SECRET"];
        curl = curl_easy_init();

        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        if (!curl) {
            throw std::runtime_error("failed to create curl handler");
        }
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Coinbase C++ API Client");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CoinbaseClient::CurlCallback);
    }

    CoinbaseClient::~CoinbaseClient() {
        curl_easy_cleanup(curl);
    }

    std::string CoinbaseClient::ListAccounts() {
        const std::string method = "GET";
        const std::string body = "";
        const std::string accounts_url = "/api/v3/brokerage/accounts";
        const auto timestamp = std::to_string(clock.GetMillisSinceEpoch());
        curl_easy_setopt(curl, CURLOPT_URL, (url + accounts_url).c_str());
        const auto accept_header = std::make_pair("accept", "application/json");
        const auto access_key_header = std::make_pair("CB-ACCESS-KEY", key);
        const auto access_sign_header = std::make_pair("CB-ACCESS-SIGN",
                                                       GenerateSignature(method, accounts_url, body, timestamp));
        const auto access_timestamp_header = std::make_pair("CB-ACCESS-TIMESTAMP", timestamp);
        curl_slist *chunk = nullptr;
        std::vector<const std::pair<std::string, std::string>> headers = {accept_header, access_key_header,
                                                                          access_sign_header, access_timestamp_header};
        for_each(headers.begin(), headers.end(), [&chunk](const auto &header) {
            chunk = curl_slist_append(chunk, (header.first + ": " + header.second).c_str());
        });
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void *>(&response));
        CURLcode result = curl_easy_perform(curl);
        curl_slist_free_all(chunk);
        if (result != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(result));
        return response;
    }

    std::string CoinbaseClient::GenerateSignature(const std::string &method, const std::string &requestPath,
                                                  const std::string &body,
                                                  const std::string &timestamp) {
        std::vector<std::string> tokens;
        boost::split(tokens, requestPath, boost::is_any_of("?"));
        const auto message = timestamp + method + requestPath + body;
        return Encryption::ToHex(Encryption::HMACSha256(message, secret), SHA256_DIGEST_LENGTH);
    }
}