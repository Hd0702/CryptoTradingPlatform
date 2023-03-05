#include "EncryptionHelper.hpp"
#include <openssl/buffer.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/bio.h>
#include <vector>

namespace Encryption {
    std::string Encryption::HMACSha512(const std::string data, const std::string key) {
        return reinterpret_cast<const char *>(HMAC(EVP_sha512(), key.data(), key.size(), reinterpret_cast<const unsigned char *>(data.data()), data.size(), nullptr, nullptr));
    }

    std::string Encryption::B64Decode(std::string data) {
        BIO* b64 = BIO_new(BIO_f_base64());
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

        BIO* bmem = BIO_new_mem_buf((void*)data.c_str(),data.length());
        bmem = BIO_push(b64, bmem);

        std::vector<char> output(data.length());
        int decoded_size = BIO_read(bmem, output.data(), output.size());
        BIO_free_all(bmem);

        if (decoded_size < 0)
            throw std::runtime_error("failed while decoding base64.");

        return output.data();
    }

    std::string Encryption::B64Encode(std::string data) {
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
}