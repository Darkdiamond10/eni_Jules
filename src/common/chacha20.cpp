#include "common/chacha20.h"
#include <cstring>

namespace Phantom {
namespace Crypto {

// Functional ChaCha20 Quarter Round macro for ASM
#define CHACHA20_QUARTERROUND_ASM(a, b, c, d) \
    "addl %%" #b ", %%" #a "\n\t" \
    "xorl %%" #a ", %%" #d "\n\t" \
    "roll $16, %%" #d "\n\t" \
    "addl %%" #d ", %%" #c "\n\t" \
    "xorl %%" #c ", %%" #b "\n\t" \
    "roll $12, %%" #b "\n\t" \
    "addl %%" #b ", %%" #a "\n\t" \
    "xorl %%" #a ", %%" #d "\n\t" \
    "roll $8, %%" #d "\n\t" \
    "addl %%" #d ", %%" #c "\n\t" \
    "xorl %%" #c ", %%" #b "\n\t" \
    "roll $7, %%" #b "\n\t"

void chacha20_block_asm(uint32_t out[16], const uint32_t key[8], const uint32_t nonce[3], uint32_t counter) {
    // Initial state setup
    uint32_t state[16];
    state[0] = 0x61707865; // "expa"
    state[1] = 0x3332206e; // "nd 3"
    state[2] = 0x79622d32; // "2-by"
    state[3] = 0x6b206574; // "te k"
    std::memcpy(&state[4], key, 32);
    state[12] = counter;
    std::memcpy(&state[13], nonce, 12);

    uint32_t x[16];
    std::memcpy(x, state, 64);

    // 20 rounds (10 iterations of 8 quarter rounds)
    for (int i = 0; i < 10; ++i) {
        // Column rounds
        // x0, x4, x8, x12
        __asm__ __volatile__ (
            "movl %[x0], %%eax\n\t"
            "movl %[x4], %%ebx\n\t"
            "movl %[x8], %%ecx\n\t"
            "movl %[x12], %%edx\n\t"
            CHACHA20_QUARTERROUND_ASM(eax, ebx, ecx, edx)
            "movl %%eax, %[x0]\n\t"
            "movl %%ebx, %[x4]\n\t"
            "movl %%ecx, %[x8]\n\t"
            "movl %%edx, %[x12]\n\t"
            : [x0] "+m" (x[0]), [x4] "+m" (x[4]), [x8] "+m" (x[8]), [x12] "+m" (x[12])
            : : "eax", "ebx", "ecx", "edx"
        );
        // x1, x5, x9, x13
        __asm__ __volatile__ (
            "movl %[x1], %%eax\n\t"
            "movl %[x5], %%ebx\n\t"
            "movl %[x9], %%ecx\n\t"
            "movl %[x13], %%edx\n\t"
            CHACHA20_QUARTERROUND_ASM(eax, ebx, ecx, edx)
            "movl %%eax, %[x1]\n\t"
            "movl %%ebx, %[x5]\n\t"
            "movl %%ecx, %[x9]\n\t"
            "movl %%edx, %[x13]\n\t"
            : [x1] "+m" (x[1]), [x5] "+m" (x[5]), [x9] "+m" (x[9]), [x13] "+m" (x[13])
            : : "eax", "ebx", "ecx", "edx"
        );
        // x2, x6, x10, x14
        __asm__ __volatile__ (
            "movl %[x2], %%eax\n\t"
            "movl %[x6], %%ebx\n\t"
            "movl %[x10], %%ecx\n\t"
            "movl %[x14], %%edx\n\t"
            CHACHA20_QUARTERROUND_ASM(eax, ebx, ecx, edx)
            "movl %%eax, %[x2]\n\t"
            "movl %%ebx, %[x6]\n\t"
            "movl %%ecx, %[x10]\n\t"
            "movl %%edx, %[x14]\n\t"
            : [x2] "+m" (x[2]), [x6] "+m" (x[6]), [x10] "+m" (x[10]), [x14] "+m" (x[14])
            : : "eax", "ebx", "ecx", "edx"
        );
        // x3, x7, x11, x15
        __asm__ __volatile__ (
            "movl %[x3], %%eax\n\t"
            "movl %[x7], %%ebx\n\t"
            "movl %[x11], %%ecx\n\t"
            "movl %[x15], %%edx\n\t"
            CHACHA20_QUARTERROUND_ASM(eax, ebx, ecx, edx)
            "movl %%eax, %[x3]\n\t"
            "movl %%ebx, %[x7]\n\t"
            "movl %%ecx, %[x11]\n\t"
            "movl %%edx, %[x15]\n\t"
            : [x3] "+m" (x[3]), [x7] "+m" (x[7]), [x11] "+m" (x[11]), [x15] "+m" (x[15])
            : : "eax", "ebx", "ecx", "edx"
        );

        // Diagonal rounds
        // x0, x5, x10, x15
        __asm__ __volatile__ (
            "movl %[x0], %%eax\n\t"
            "movl %[x5], %%ebx\n\t"
            "movl %[x10], %%ecx\n\t"
            "movl %[x15], %%edx\n\t"
            CHACHA20_QUARTERROUND_ASM(eax, ebx, ecx, edx)
            "movl %%eax, %[x0]\n\t"
            "movl %%ebx, %[x5]\n\t"
            "movl %%ecx, %[x10]\n\t"
            "movl %%edx, %[x15]\n\t"
            : [x0] "+m" (x[0]), [x5] "+m" (x[5]), [x10] "+m" (x[10]), [x15] "+m" (x[15])
            : : "eax", "ebx", "ecx", "edx"
        );
        // x1, x6, x11, x12
        __asm__ __volatile__ (
            "movl %[x1], %%eax\n\t"
            "movl %[x6], %%ebx\n\t"
            "movl %[x11], %%ecx\n\t"
            "movl %[x12], %%edx\n\t"
            CHACHA20_QUARTERROUND_ASM(eax, ebx, ecx, edx)
            "movl %%eax, %[x1]\n\t"
            "movl %%ebx, %[x6]\n\t"
            "movl %%ecx, %[x11]\n\t"
            "movl %%edx, %[x12]\n\t"
            : [x1] "+m" (x[1]), [x6] "+m" (x[6]), [x11] "+m" (x[11]), [x12] "+m" (x[12])
            : : "eax", "ebx", "ecx", "edx"
        );
        // x2, x7, x8, x13
        __asm__ __volatile__ (
            "movl %[x2], %%eax\n\t"
            "movl %[x7], %%ebx\n\t"
            "movl %[x8], %%ecx\n\t"
            "movl %[x13], %%edx\n\t"
            CHACHA20_QUARTERROUND_ASM(eax, ebx, ecx, edx)
            "movl %%eax, %[x2]\n\t"
            "movl %%ebx, %[x7]\n\t"
            "movl %%ecx, %[x8]\n\t"
            "movl %%edx, %[x13]\n\t"
            : [x2] "+m" (x[2]), [x7] "+m" (x[7]), [x8] "+m" (x[8]), [x13] "+m" (x[13])
            : : "eax", "ebx", "ecx", "edx"
        );
        // x3, x4, x9, x14
        __asm__ __volatile__ (
            "movl %[x3], %%eax\n\t"
            "movl %[x4], %%ebx\n\t"
            "movl %[x9], %%ecx\n\t"
            "movl %[x14], %%edx\n\t"
            CHACHA20_QUARTERROUND_ASM(eax, ebx, ecx, edx)
            "movl %%eax, %[x3]\n\t"
            "movl %%ebx, %[x4]\n\t"
            "movl %%ecx, %[x9]\n\t"
            "movl %%edx, %[x14]\n\t"
            : [x3] "+m" (x[3]), [x4] "+m" (x[4]), [x9] "+m" (x[9]), [x14] "+m" (x[14])
            : : "eax", "ebx", "ecx", "edx"
        );
    }

    for (int i = 0; i < 16; ++i) {
        out[i] = x[i] + state[i];
    }

    // WIPE intermediate state
    std::memset(state, 0, sizeof(state));
    std::memset(x, 0, sizeof(x));
}

void chacha20_xor(uint8_t* data, size_t len, const uint32_t key[8], const uint32_t nonce[3], uint32_t counter) {
    uint32_t block[16];
    while (len > 0) {
        chacha20_block_asm(block, key, nonce, counter);
        size_t take = (len < 64) ? len : 64;
        uint8_t* block8 = (uint8_t*)block;
        for (size_t i = 0; i < take; ++i) {
            data[i] ^= block8[i];
        }
        data += take;
        len -= take;
        counter++;
    }
    std::memset(block, 0, sizeof(block));
}

}
}
