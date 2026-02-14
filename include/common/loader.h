#ifndef PHANTOM_LOADER_H
#define PHANTOM_LOADER_H

#include <cstdint>
#include <cstddef>
#include <vector>

namespace Phantom {
namespace Loader {

    // Reflectively loads and executes an ELF binary from memory.
    // This avoids fexecve and remains entirely fileless.
    typedef int (*entry_t)(int, char**, char**);

    bool reflective_load(const uint8_t* elf_data, size_t size);

}
}

#endif // PHANTOM_LOADER_H
