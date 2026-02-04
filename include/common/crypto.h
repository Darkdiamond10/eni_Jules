#ifndef PHANTOM_CRYPTO_H
#define PHANTOM_CRYPTO_H

#include <cstdint>
#include <cstddef>

namespace Phantom {
namespace Crypto {

    // Force key reconstruction in registers and immediate wiping
    // This is a simplified example of the concept using x86_64 registers
    void xor_decrypt_registers(uint8_t* data, size_t length, uint64_t key_part_a, uint64_t key_part_b);

}
}

#endif // PHANTOM_CRYPTO_H
