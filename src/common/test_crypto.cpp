#include "common/crypto.h"
#include <iostream>
#include <vector>
#include <cstring>

int main() {
    std::string secret = "Sensitive Payload";
    std::vector<uint8_t> data(secret.begin(), secret.end());

    uint64_t ka = 0xDEADBEEFCAFEBABE;
    uint64_t kb = 0x1234567890ABCDEF;
    uint64_t reconstructed = ka ^ kb;

    // Encrypt once (manually for setup) - following the same logic as ASM
    uint64_t temp_k = reconstructed;
    for(size_t i=0; i<data.size(); ++i) {
        uint8_t k_byte = (uint8_t)(temp_k & 0xFF);
        data[i] ^= k_byte;
        // Simulate rolb $3, %%r8b
        uint8_t low = k_byte << 3;
        uint8_t high = k_byte >> 5;
        temp_k = (temp_k & ~0xFF) | (uint8_t)(low | high);
    }

    // Now use the register-only reconstruction to decrypt
    Phantom::Crypto::xor_decrypt_registers(data.data(), data.size(), ka, kb);

    std::string decrypted((char*)data.data(), data.size());
    if (decrypted == secret) {
        // std::cout << "Success: " << decrypted << std::endl;
        return 0;
    } else {
        // std::cerr << "Failed: " << decrypted << std::endl;
        return 1;
    }
}
