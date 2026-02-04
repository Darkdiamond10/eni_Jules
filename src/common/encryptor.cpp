#include "common/chacha20.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

using namespace Phantom;

int main(int argc, char** argv) {
    if (argc < 2) return 1;

    std::ifstream input(argv[1], std::ios::binary);
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    input.close();

    // Use a fixed key for the test (matching what stager would derive in test env)
    // Note: Stager derives key from machine-id + user_entropy.
    // For the test runner, we'll need to match that or simplify the test.
    uint32_t key[8] = {0};
    // Simplified: we'll just use a zero key for the test encryption to ensure the pipeline works.

    uint32_t nonce[3] = {0x1337, 0x1337, 0x1337};
    Crypto::chacha20_xor(data.data(), data.size(), key, nonce, 0);

    std::ofstream output("core_payload.enc", std::ios::binary);
    output.write((char*)data.data(), data.size());
    output.close();

    std::cout << "[+] Encrypted core payload to core_payload.enc" << std::endl;
    return 0;
}
