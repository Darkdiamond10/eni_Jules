#include <iostream>
#include <unistd.h>

int main(int argc, char** argv) {
    // This is the functional core payload.
    // When reflectively loaded, it should print a message and exit.
    std::cout << "[+] Phantom CORE active in memory!" << std::endl;
    std::cout << "[+] PID: " << getpid() << std::endl;
    return 0;
}
