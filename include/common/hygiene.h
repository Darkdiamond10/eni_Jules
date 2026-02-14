#ifndef PHANTOM_HYGIENE_H
#define PHANTOM_HYGIENE_H

#include <cstddef>

namespace Phantom {
namespace Hygiene {

    // Securely wipe a memory region
    void secure_wipe(void* ptr, size_t size);

    // Perform 'Cryptographic Suicide'
    void self_destruct();

}
}

#endif // PHANTOM_HYGIENE_H
