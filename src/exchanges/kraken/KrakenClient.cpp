#include <openssl/sha.h>
#include <sstream>

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

    std::string KrakenClient::buy() const {
        // hardcoded values for testing purposes replace later
        const std::string pair = "pair: ETHUSDT";
        const std::string volume = "volume: 0.01";
        const std::string type = "type: sell";
        const std::string orderType = "ordertype: market";

        // set body
        curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, pair.c_str());
        headers = curl_slist_append(headers, volume.c_str());
        headers = curl_slist_append(headers, type.c_str());
        headers = curl_slist_append(headers, orderType.c_str());

        return makePrivateCall("/0/private/AddOrder", headers);
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

    std::string KrakenClient::makePrivateCall(const std::string &pathSuffix, curl_slist *headers) const {
        std::string nonce = this->nonce();
        const std::string fullPath = std::string(KrakenClient::url) + pathSuffix;
        curl_easy_setopt(curl, CURLOPT_URL, fullPath.c_str());
        std::string postdata = "nonce=" + nonce;
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata.c_str());
        std::string key_header = "API-Key: " + key;
        std::string sign_header = "API-Sign: " + signature(pathSuffix, nonce, postdata);
        headers = curl_slist_append(headers, key_header.c_str());
        headers = curl_slist_append(headers, sign_header.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // where CURL write callback function stores the response
        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void *>(&response));
        // perform CURL request
        // commenting out to check hash
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