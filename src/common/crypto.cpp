#include "common/crypto.h"

namespace Phantom {
namespace Crypto {

void xor_decrypt_registers(uint8_t* data, size_t length, uint64_t key_part_a, uint64_t key_part_b) {
    // We use volatile to ensure the compiler doesn't optimize out the register usage
    // or the zeroing.

    // In a production implant, we'd use more complex algorithms (AES-NI),
    // but the principle of register-only reconstruction remains.

    __asm__ __volatile__ (
        "movq %[ka], %%r8\n\t"    // Load part A into r8
        "movq %[kb], %%r9\n\t"    // Load part B into r9
        "xorq %%r9, %%r8\n\t"     // Reconstruct key in r8 (ka ^ kb)

        // Simple XOR loop over data
        "testq %[len], %[len]\n\t"
        "jz 2f\n\t"
        "movq $0, %%rcx\n\t"
        "1:\n\t"
        "movb (%[ptr], %%rcx), %%al\n\t"
        "xorb %%r8b, %%al\n\t"
        "rolb $3, %%r8b\n\t"      // Rotate key byte to increase diffusion
        "movb %%al, (%[ptr], %%rcx)\n\t"
        "incq %%rcx\n\t"
        "cmpq %[len], %%rcx\n\t"
        "jne 1b\n\t"

        "2:\n\t"
        // WIPE REGISTERS IMMEDIATELY
        "xorq %%r8, %%r8\n\t"
        "xorq %%r9, %%r9\n\t"
        "xorq %%rax, %%rax\n\t"
        "xorq %%rcx, %%rcx\n\t"
        "xorq %%rdx, %%rdx\n\t"

        : // No outputs
        : [ptr] "r" (data), [len] "r" (length), [ka] "r" (key_part_a), [kb] "r" (key_part_b)
        : "r8", "r9", "rax", "rcx", "rdx", "memory"
    );
}

}
}
