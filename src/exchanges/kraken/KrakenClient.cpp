#include <numeric>
#include <sstream>
#include <nlohmann/json.hpp>

#include "KrakenClient.hpp"

#include <iostream>

#include "../../encryption/EncryptionHelper.hpp"
#include "KrakenOHLC.hpp"

namespace Kraken {

    nlohmann::json parseAndThrowErrors(std::string krakenResult) {
        auto jsonResult = nlohmann::json::parse(krakenResult);
        std::vector<std::string> errors = jsonResult.at("error");
        if (!errors.empty()) {
            throw std::runtime_error(std::string("Errors returned from Kraken: ")
                                     + std::accumulate(std::next(errors.cbegin()), errors.cend(), errors[0], [](const std::string& accum, const std::string& error) { return accum + ", " + error; }));
        }
        return jsonResult.at("result");
    }

    KrakenClient::KrakenClient(Env::EnvReader env_reader_instance) : env_reader(std::move(env_reader_instance)) { Init(); }

    KrakenClient::KrakenClient() : env_reader(Env::EnvReader(".env")) { Init(); }


    void KrakenClient::Init() {

        key = env_reader["KRAKEN_API_KEY"];
        secret = env_reader["KRAKEN_API_SECRET"];

        curl = curl_easy_init();
        if (!curl) {
            throw std::runtime_error("failed to create curl handler");
        }
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Kraken C++ API Client");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, KrakenClient::curlCallback);
    }

    KrakenClient::~KrakenClient() {
        curl_easy_cleanup(curl);
    }

    double KrakenClient::getTicker(const std::string &pair) const {
        const auto path = std::string(KrakenClient::url) + std::string(KrakenClient::tickerURL) + "?pair=" + pair;
        auto resultString = makePublicCall(path);
        nlohmann::json jsonResult = parseAndThrowErrors(resultString).at(pair).at("c");
        if (!jsonResult.is_array()) throw std::runtime_error("Could not parse array from ticket json result in Kraken.");
        return std::stod(jsonResult.at(0).get<std::string>());
    }

    std::vector<KrakenTrade> KrakenClient::getTrades(const long long epochSeconds, const std::string &pair) const {
        const auto path = std::string(KrakenClient::url) + std::string(KrakenClient::tradesURL) + "?pair=" + pair + "&since=" + std::to_string(epochSeconds);
        auto resultString = makePublicCall(path);
        nlohmann::json jsonResult = parseAndThrowErrors(resultString).at(pair);
        std::vector<KrakenTrade> result;
        result.reserve(jsonResult.size());
        std::transform(jsonResult.begin(), jsonResult.end(), std::back_inserter(result), [](const nlohmann::json& item) { return item.template get<KrakenTrade>(); });
        return result;
    }

    std::map<std::string, KrakenTradeInfo> KrakenClient::getTradeInfo(const std::vector<std::string>&txIds) const {
        int chunked = 0;
        std::map<std::string, KrakenTradeInfo> result;
        while (chunked < txIds.size()) {
            int group = std::min(20, static_cast<int>(txIds.size()) - chunked);
            auto start = txIds.cbegin() + chunked;
            auto end = txIds.cbegin() + chunked + group;
            std::string txIdsString = std::accumulate(start, end, std::string(), [](const std::string& accum, const std::string& txId) { return accum + txId + ","; });
            txIdsString.pop_back();
            chunked += 20;
            auto resultString = makePrivateCall(std::string(KrakenClient::queryTradeURL), nullptr, {std::make_pair("txid", txIdsString)});
            nlohmann::json jsonResult = parseAndThrowErrors(resultString);
            std::map<std::string, KrakenTradeInfo> smallResult = jsonResult.get<std::map<std::string, KrakenTradeInfo>>();
            result.insert(std::make_move_iterator(smallResult.begin()), std::make_move_iterator(smallResult.end()));
        }
        return result;
    }


    std::vector<std::unique_ptr<BaseOHLC>> KrakenClient::getOHLC(const long long epochNanos, const std::string& pair) const {
        const auto path = std::string(KrakenClient::url) + std::string(KrakenClient::ohlcURL) + "?pair=" + pair + "&since=" + std::to_string(epochNanos);
        auto resultString = makePublicCall(path);
        nlohmann::json jsonResult = parseAndThrowErrors(resultString).at(pair);
        std::vector<std::unique_ptr<BaseOHLC>> result;
        result.reserve(jsonResult.size());
        std::transform(jsonResult.begin(), jsonResult.end(), std::back_inserter(result), [](const nlohmann::json& item) { return std::make_unique<KrakenOHLC>(item.template get<KrakenOHLC>()); });
        return result;
    }

    KrakenOrder KrakenClient::buy(const std::string& pair, const std::string& volume, const std::string& type, const std::string& orderType, bool dryRun) const {
        curl_slist* headers = nullptr;
        std::vector<const std::pair<std::string, std::string>> postData = {
                {"pair", pair}, {"volume",  volume}, {"type", type}, {"ordertype", orderType}, {"validate", dryRun ? "true" : "false"}
        };

        std::string result = makePrivateCall("/0/private/AddOrder", headers, postData);
        nlohmann::json jsonResult = parseAndThrowErrors(result);
        // is it worth turning into an object?
        return jsonResult.get<KrakenOrder>();
    }

    std::string KrakenClient::nonce() const {
        return std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count());
    }

    std::string KrakenClient::getServerTime() {
        return makePublicCall(std::string(KrakenClient::url) + std::string(KrakenClient::timeURL));
    }

    std::string KrakenClient::signature(const std::string &path,
                                        const std::string &nonce,
                                        const std::string &postData) const {
        // add path to data to encrypt
        std::string data(path.begin(), path.end());

        // concatenate nonce and postdata and compute SHA256
        std::vector<unsigned char> nonce_postdata = Encryption::hmacSha256(nonce + postData);

        // concatenate path and nonce_postdata (path + sha256(nonce + postdata))
        data += std::string(nonce_postdata.begin(), nonce_postdata.end());

        // and compute HMAC
        auto decoded = Encryption::b64Decode(secret);
        auto output = Encryption::b64Encode(Encryption::hmacSha512(data, decoded));
        return Encryption::b64Encode(Encryption::hmacSha512(data, decoded));
    }

    size_t KrakenClient::curlCallback(char *ptr, size_t size, size_t nmemb, void *userdata) {
        auto response = static_cast<std::string *>(userdata);
        auto real_size = size * nmemb;

        response->append(ptr, real_size);
        return real_size;
    }

    std::string KrakenClient::getBalance() {
        std::string pathSuffix = "/0/private/TradeBalance";
        return makePrivateCall(pathSuffix, nullptr);
    }

    std::string KrakenClient::makePublicCall(const std::string& path, const std::unique_ptr<curl_slist> &headers, const std::string& postdata) const {
        curl_easy_setopt(curl, CURLOPT_URL, path.c_str());
        std::string response;
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers.get());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void *>(&response));
        CURLcode result = curl_easy_perform(curl);
        if (result != CURLE_OK) {
            std::ostringstream oss;
            oss << "curl_easy_perform() failed: " << curl_easy_strerror(result);
            throw std::runtime_error(oss.str());
        }
        return response;
    }

    std::string KrakenClient::makePrivateCall(const std::string &pathSuffix, curl_slist *headers, std::vector<const std::pair<std::string, std::string>> postData) const {
        std::string nonce = this->nonce();
        const std::string fullPath = std::string(KrakenClient::url) + pathSuffix;
        curl_easy_setopt(curl, CURLOPT_URL, fullPath.c_str());
        // always have the nonce be the last postfield for formatting
        std::string postDataString;
        auto transformPostDataFunc = [&](const std::pair<std::string, std::string>& nameAndData) {
            return nameAndData.first + '=' + nameAndData.second + '&';
        };
        if (!postData.empty()) {
            postDataString += std::reduce(postData.begin() + 1, postData.end(), transformPostDataFunc(*postData.cbegin()), [&](std::string res, const std::pair<std::string, std::string> nxt){
                res += transformPostDataFunc(nxt);
                return res;
            });
        }
        postDataString += "nonce=" + nonce;
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postDataString.c_str());
        std::string key_header = "API-Key: " + key;
        std::string sign_header = "API-Sign: " + signature(pathSuffix, nonce, postDataString);
        headers = curl_slist_append(headers, key_header.c_str());
        headers = curl_slist_append(headers, sign_header.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // where CURL write callback function stores the response
        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void *>(&response));
        // perform CURL request
        CURLcode result = curl_easy_perform(curl);

        // free the custom headers
        curl_slist_free_all(headers);

        // check perform result
        if (result != CURLE_OK) {
            std::ostringstream oss;
            oss << "curl_easy_perform() failed: " << curl_easy_strerror(result);
            throw std::runtime_error(oss.str());
        }
        return response;
    }
}