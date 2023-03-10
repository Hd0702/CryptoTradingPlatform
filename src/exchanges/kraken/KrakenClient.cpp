#include <boost/date_time/posix_time/posix_time_io.hpp>

#include <openssl/sha.h>

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
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Kraken C++ API Client");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, KrakenClient::CurlCallback);
    }

    KrakenClient::~KrakenClient() {
        curl_easy_cleanup(curl);
    }

    const std::string KrakenClient::GenerateNonce() {
        return std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count());
    }

    std::string KrakenClient::GetServerTime() {
        const auto path = std::string(KrakenClient::url) + std::string(KrakenClient::timeURL);
        curl_easy_setopt(curl, CURLOPT_URL, path.c_str());
        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void *>(&response));
        CURLcode result = curl_easy_perform(curl);
        if (result != CURLE_OK) {
            std::ostringstream oss;
            oss << "curl_easy_perform() failed: " << curl_easy_strerror(result);
            throw std::runtime_error(oss.str());
        }
        return response;
    }

    std::string KrakenClient::Signature(const std::string &path,
                                        const std::string &nonce,
                                        const std::string &postData) const {
        // add path to data to encrypt
        std::string data(path.begin(), path.end());

        // concatenate nonce and postdata and compute SHA256
        std::vector<unsigned char> nonce_postdata = Encryption::HMACSha256(nonce + postData);

        // concatenate path and nonce_postdata (path + sha256(nonce + postdata))
        data += std::string(nonce_postdata.begin(), nonce_postdata.end());

        // and compute HMAC
        auto decoded = Encryption::B64Decode(secret);
        return Encryption::B64Encode(Encryption::HMACSha512(data, decoded));
    }

    size_t KrakenClient::CurlCallback(char *ptr, size_t size, size_t nmemb, void *userdata) {
        auto response = static_cast<std::string *>(userdata);
        auto real_size = size * nmemb;

        response->append(ptr, real_size);
        return real_size;
    }

    std::string KrakenClient::GetBalance() {
        std::string path = "/0/private/TradeBalance";
        std::string full_path = std::string(KrakenClient::url) + path;
        std::string nonce = GenerateNonce();
        curl_easy_setopt(curl, CURLOPT_URL, full_path.c_str());
        std::string postdata = "nonce=" + nonce;
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata.c_str());

        // add custom header
        curl_slist *chunk = NULL;

        std::string key_header = "API-Key: " + key;
        std::string sign_header = "API-Sign: " + Signature(path, nonce, postdata);

        chunk = curl_slist_append(chunk, key_header.c_str());
        chunk = curl_slist_append(chunk, sign_header.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

        // where CURL write callback function stores the response
        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void *>(&response));
        // perform CURL request
        CURLcode result = curl_easy_perform(curl);

        // free the custom headers
        curl_slist_free_all(chunk);

        // check perform result
        if (result != CURLE_OK) {
            std::ostringstream oss;
            oss << "curl_easy_perform() failed: " << curl_easy_strerror(result);
            throw std::runtime_error(oss.str());
        }
        return response;
    }
}