#include "common/hygiene.h"
#include <cstring>
#include <random>
#include <unistd.h>
#include <sys/mman.h>

namespace Phantom {
namespace Hygiene {

void secure_wipe(void* ptr, size_t size) {
    if (!ptr || size == 0) return;

    // 1. Overwrite with random data
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint8_t> dis(0, 255);

    uint8_t* byte_ptr = static_cast<uint8_t*>(ptr);
    for (size_t i = 0; i < size; ++i) {
        byte_ptr[i] = dis(gen);
    }

    // 2. Overwrite with zeros
    std::memset(ptr, 0, size);

    // 3. Ensure the compiler doesn't optimize this away
    __asm__ __volatile__("" : : "r"(ptr) : "memory");
}

void self_destruct() {
    // In a real scenario, this would iterate over all sensitive memory regions
    // and then call exit(0) or similar.

    // We can also use MADV_DONTDUMP to advise the kernel not to include this in core dumps
    // madvise(ptr, size, MADV_DONTDUMP);

    // After wiping, we terminate.
    _exit(0);
}

}
}
