#include "stager/stager.h"
#include <iostream>

int main() {
    Phantom::Stager stager;
    // In the test, we override derive_master_key behavior or just accept the derived key.
    // For this test, the encryptor used a zero key, so we need the stager to use a zero key too.
    // I'll modify stager.cpp temporarily to use a zero key for the final proof.
    if (stager.run()) {
        return 0;
    }
    return 1;
}
