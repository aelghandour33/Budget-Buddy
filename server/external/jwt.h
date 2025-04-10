#pragma once

#include <string>
#include <map>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstring>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

namespace jwt {

namespace params {
    struct algorithm {
        std::string alg;
        algorithm(const std::string& a) : alg(a) {}
    };
    struct secret {
        std::string key;
        secret(const std::string& s) : key(s) {}
    };
        inline algorithm algorithms(const std::string& a) { return algorithm(a); }

}

class jwt_object {
public:
    std::map<std::string, std::string> claims;
    std::string alg = "HS256";
    std::string secretKey;

    jwt_object(const params::algorithm& a, const params::secret& s)
        : alg(a.alg), secretKey(s.key) {}

    void add_claim(const std::string& key, const std::string& value) {
        claims[key] = value;
    }
    std::map<std::string, std::string> payload() const {
        return claims;
    }

    std::string base64_encode(const std::string& input) {
        BIO* b64 = BIO_new(BIO_f_base64());
        BIO* bio = BIO_new(BIO_s_mem());
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
        bio = BIO_push(b64, bio);

        BIO_write(bio, input.c_str(), input.size());
        BIO_flush(bio);

        BUF_MEM* bufferPtr;
        BIO_get_mem_ptr(bio, &bufferPtr);
        std::string encoded(bufferPtr->data, bufferPtr->length);

        BIO_free_all(bio);
        return encoded;
    }

    std::string hmac_sha256(const std::string& data) {
        unsigned char* digest;
        digest = HMAC(EVP_sha256(),
                      secretKey.c_str(), secretKey.length(),
                      (unsigned char*)data.c_str(), data.length(),
                      NULL, NULL);
        char mdString[65];
        for (int i = 0; i < 32; i++)
            sprintf(&mdString[i * 2], "%02x", (unsigned int)digest[i]);
        return std::string(mdString);
    }

    std::string signature() {
        std::string header = R"({"alg":")" + alg + R"(","typ":"JWT"})";
        std::string payload = "{";
        bool first = true;
        for (const auto& pair : claims) {
            if (!first) payload += ",";
            payload += "\"" + pair.first + "\":\"" + pair.second + "\"";
            first = false;
        }
        payload += "}";

        std::string encoded_header = base64_encode(header);
        std::string encoded_payload = base64_encode(payload);
        std::string data = encoded_header + "." + encoded_payload;
        std::string sig = hmac_sha256(data);
        return data + "." + sig;
    }
};

inline jwt_object decode(const std::string& token, const params::algorithm&, const params::secret& s) {
    size_t first_dot = token.find('.');
    size_t second_dot = token.find('.', first_dot + 1);
    if (first_dot == std::string::npos || second_dot == std::string::npos)
        throw std::runtime_error("Invalid token format");

    std::string header = token.substr(0, first_dot);
    std::string payload = token.substr(first_dot + 1, second_dot - first_dot - 1);
    std::string signature = token.substr(second_dot + 1);

    std::string data = header + "." + payload;

    jwt_object obj(params::algorithm("HS256"), s);
    std::string expected_sig = obj.hmac_sha256(data);

    if (expected_sig != signature)
        throw std::runtime_error("Invalid token signature");

    return obj;
}

} // namespace jwt
