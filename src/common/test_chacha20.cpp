#include "common/chacha20.h"
#include <iostream>
#include <vector>
#include <iomanip>

void print_hex(const uint8_t* data, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)data[i];
    }
    std::cout << std::dec << std::endl;
}

int main() {
    uint32_t key[8] = {0x00010203, 0x04050607, 0x08090a0b, 0x0c0d0e0f, 0x10111213, 0x14151617, 0x18191a1b, 0x1c1d1e1f};
    uint32_t nonce[3] = {0x00000000, 0x00000000, 0x00000000};
    uint32_t counter = 0;

    std::string plaintext = "The quick brown fox jumps over the lazy dog";
    std::vector<uint8_t> data(plaintext.begin(), plaintext.end());

    std::cout << "Original: " << plaintext << std::endl;

    // Encrypt
    Phantom::Crypto::chacha20_xor(data.data(), data.size(), key, nonce, counter);
    std::cout << "Encrypted (hex): ";
    print_hex(data.data(), data.size());

    // Decrypt
    Phantom::Crypto::chacha20_xor(data.data(), data.size(), key, nonce, counter);
    std::string decrypted((char*)data.data(), data.size());
    std::cout << "Decrypted: " << decrypted << std::endl;

    if (plaintext == decrypted) {
        std::cout << "SUCCESS: ChaCha20 ASM works!" << std::endl;
        return 0;
    } else {
        std::cout << "FAILURE: Decryption mismatch." << std::endl;
        return 1;
    }
}
