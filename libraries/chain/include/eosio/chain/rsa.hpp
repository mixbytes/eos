#pragma once
#include <memory>
#include <vector>
#include <fc/crypto/sha1.hpp>
#include <fc/crypto/sha256.hpp>
#include <fc/io/raw_fwd.hpp>
#include <fc/array.hpp>
#include <fc/crypto/base64.hpp>

#include <openssl/rsa.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#include <memory>
#include <fc/io/sstream.hpp>

namespace fc {
    typedef std::vector<char> bytes;
    typedef bytes             rsa_signature;

    class rsa_public_key {
    public:
        rsa_public_key() = default;
        explicit rsa_public_key( const std::string& b64) {
            std::string pem = "-----BEGIN PUBLIC KEY-----\n";
            for (size_t i = 0; i < b64.size(); i += 64) {
                pem += b64.substr(i, 64) + "\n";
            }
            pem += "-----END PUBLIC KEY-----\n";

            BIO* mem = (BIO*)BIO_new_mem_buf( (void*)pem.c_str(), pem.size() );
            rsa.reset(PEM_read_bio_RSA_PUBKEY(mem, NULL, NULL, NULL));
            BIO_free(mem);
        }

        operator bool() const {
            return !!rsa;
        }

        bool verify( const sha256& digest, const std::string& sig ) const {
            return 0 != RSA_verify( NID_sha256, (const unsigned char*)&digest, 32,
                                    (const unsigned char*)sig.data(), 2048/8, rsa.get());
        }
    private:
        std::shared_ptr<RSA> rsa;
    };

    namespace raw
    {
        template<typename Stream>
        void unpack( Stream& s, fc::rsa_public_key& pk)
        {
            bytes ser;
            fc::raw::unpack(s, ser);
            pk = rsa_public_key(std::string(ser.begin(), ser.end()));
        }
    }
} // fc

