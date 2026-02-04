#ifndef PHANTOM_CHACHA20_H
#define PHANTOM_CHACHA20_H

#include <cstdint>
#include <cstddef>

namespace Phantom {
namespace Crypto {

    // Functional ChaCha20 block function implemented in x86_64 assembly.
    // This function processes a 64-byte block.
    // key: 32 bytes (8 x 32-bit words)
    // nonce: 12 bytes (3 x 32-bit words)
    // counter: 4 bytes (1 x 32-bit word)
    void chacha20_block_asm(uint32_t out[16], const uint32_t key[8], const uint32_t nonce[3], uint32_t counter);

    // High-level XOR stream function
    void chacha20_xor(uint8_t* data, size_t len, const uint32_t key[8], const uint32_t nonce[3], uint32_t counter);

}
}

#endif // PHANTOM_CHACHA20_H
