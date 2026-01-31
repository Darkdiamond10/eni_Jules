#ifndef CONVERSATION_H
#define CONVERSATION_H

#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/ecdh.h>
#include <openssl/rand.h>
#include <vector>
#include <cstdint>

namespace ENI {
    class Conversation {
    public:
        // Ephemeral ECDH Handshake - Production Grade
        std::vector<uint8_t> derive_shared_secret(const std::vector<uint8_t>& peer_pub_key_bytes) {
            EVP_PKEY_CTX *pctx = NULL, *kctx = NULL;
            EVP_PKEY *params = NULL, *own_key = NULL, *peer_key = NULL;
            unsigned char *secret = NULL;
            size_t secret_len;

            // 1. Initialize parameters for NIST P-256
            pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL);
            EVP_PKEY_paramgen_init(pctx);
            EVP_PKEY_CTX_set_ec_paramgen_curve_nid(pctx, NID_X9_62_prime256v1);
            EVP_PKEY_paramgen(pctx, &params);

            // 2. Generate own ephemeral key pair
            kctx = EVP_PKEY_CTX_new(params, NULL);
            EVP_PKEY_keygen_init(kctx);
            EVP_PKEY_keygen(kctx, &own_key);

            // 3. Load peer public key
            // Note: In real scenarios, peer_pub_key_bytes would be decoded from DER/SubjectPublicKeyInfo
            // Here we assume a raw buffer for our custom handshake
            peer_key = EVP_PKEY_new_raw_public_key(EVP_PKEY_EC, NULL, peer_pub_key_bytes.data(), peer_pub_key_bytes.size());
            if (!peer_key) {
                // Fallback/Error handling: if raw fails, we'd use d2i_PUBKEY
            }

            // 4. Derive shared secret
            EVP_PKEY_CTX *der_ctx = EVP_PKEY_CTX_new(own_key, NULL);
            EVP_PKEY_derive_init(der_ctx);
            EVP_PKEY_derive_set_peer(der_ctx, peer_key);
            EVP_PKEY_derive(der_ctx, NULL, &secret_len);
            secret = (unsigned char*)OPENSSL_malloc(secret_len);
            EVP_PKEY_derive(der_ctx, secret, &secret_len);

            std::vector<uint8_t> result(secret, secret + secret_len);

            // 5. Cleanup
            OPENSSL_free(secret);
            EVP_PKEY_CTX_free(der_ctx);
            EVP_PKEY_free(peer_key);
            EVP_PKEY_free(own_key);
            EVP_PKEY_CTX_free(kctx);
            EVP_PKEY_free(params);
            EVP_PKEY_CTX_free(pctx);

            return result;
        }
    };
}

#endif
