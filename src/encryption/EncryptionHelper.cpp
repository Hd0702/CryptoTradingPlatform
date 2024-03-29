#include "EncryptionHelper.hpp"
#include <openssl/buffer.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/bio.h>
#include <vector>
#include <boost/algorithm/hex.hpp>

namespace Encryption {
    std::string Encryption::hmacSha512(const std::string data, const std::string key) {
        std::vector<unsigned char> result(EVP_MAX_MD_SIZE);
        reinterpret_cast<const char *>(HMAC(EVP_sha512(), key.data(), key.size(), reinterpret_cast<const unsigned char *>(data.data()), data.size(), result.data(), nullptr));
        return { result.begin(), result.end()};
    }

    std::string Encryption::b64Decode(std::string data) {
        BIO* b64 = BIO_new(BIO_f_base64());
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

        BIO* bmem = BIO_new_mem_buf((void*)data.c_str(),data.length());
        bmem = BIO_push(b64, bmem);

        std::vector<char> output(data.length());
        int decoded_size = BIO_read(bmem, output.data(), output.size());
        BIO_free_all(bmem);

        if (decoded_size < 0) {
            throw std::runtime_error("failed while decoding base64.");
        }

        return output.data();
    }

    std::string Encryption::b64Encode(std::string data) {
        BIO* b64 = BIO_new(BIO_f_base64());
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

        BIO* bmem = BIO_new(BIO_s_mem());
        b64 = BIO_push(b64, bmem);

        BIO_write(b64, data.c_str(), 64);
        BIO_flush(b64);

        BUF_MEM* bptr = NULL;
        BIO_get_mem_ptr(b64, &bptr);

        std::string output(bptr->data, bptr->length);
        BIO_free_all(b64);

        return output;
    }

    // Returns a hash as unsigned chars into a string. For further transformations please use toHex to transform into a hex value.
    std::string Encryption::hmacSha256(std::string data, std::string key, unsigned int *result_len) {
        std::vector<unsigned char> result(SHA256_DIGEST_LENGTH);
        reinterpret_cast<const char *>(HMAC(EVP_sha256(), key.data(), key.size(), reinterpret_cast<const unsigned char *>(data.data()), data.size(), result.data(), result_len));
        return { result.begin(), result.end() };
    }

    std::vector<unsigned char> Encryption::hmacSha256(const std::string& data) {
        std::vector<unsigned char> digest(SHA256_DIGEST_LENGTH);
        unsigned char * d = SHA256(reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), nullptr);
        for (int i =0; i < SHA256_DIGEST_LENGTH; i++) {
            digest[i] = d[i];
        }

        return digest;
    }

    std::string Encryption::toHex(std::string data, size_t digest_len) {
        std::string result;
        boost::algorithm::hex_lower(data.begin(), data.end(), std::back_inserter(result));
        return result;
    }
}