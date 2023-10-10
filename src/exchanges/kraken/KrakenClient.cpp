#include <numeric>
#include <sstream>
#include <boost/json/src.hpp>

#include "KrakenClient.hpp"
#include "../../encryption/EncryptionHelper.hpp"

namespace Kraken {
    KrakenClient::KrakenClient(Env::EnvReader &env_reader_instance) : env_reader(env_reader_instance) { Init(); }

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

    // for now we save trade data into a file
    // Kraken requests timestamps in milliseconds
    std::string KrakenClient::getTradesSince(const long long epochMillis, const std::string pair) const {
        const auto path = std::string(KrakenClient::url) + std::string(KrakenClient::tradesURL) + "?pair=" + pair + "&since=" + std::to_string(epochMillis);
        return makePublicCall(path);
    }

    std::string KrakenClient::buy(const std::string& pair, const std::string& volume, const std::string& type, const std::string& orderType) const {
        curl_slist* headers = nullptr;
        std::vector<const std::pair<std::string, std::string>> postData = {
                std::make_pair("pair", pair), std::make_pair("volume",  volume), std::make_pair("type", type), std::make_pair("ordertype", orderType)
        };

        std::string result = makePrivateCall("/0/private/AddOrder", headers, postData);
        boost::json::value val = boost::json::parse(result);
        boost::json::array errors = val.at("error").as_array();
        if (!errors.empty()) {
            throw std::runtime_error(std::string("Error buying Kraken order ") + errors.front().as_string().c_str());
        }
        return result;
    }

    const std::string KrakenClient::nonce() const {
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