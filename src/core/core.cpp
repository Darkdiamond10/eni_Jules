#include "core/core.h"
#include <iostream>
#include <cmath>

namespace Phantom {

Core::Core() : m_running(true) {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    m_generator.seed(seed);
}

Core::~Core() {}

double Core::get_poisson_delay(double lambda) {
    // Lambda is the average number of events per interval
    // Here we use it to calculate the inter-arrival time
    std::exponential_distribution<double> distribution(lambda);
    return distribution(m_generator);
}

void Core::send_heartbeat() {
    // Simulate QUIC/UDP packet sending
    // std::cout << "[*] Sending encrypted heartbeat via QUIC..." << std::endl;
}

void Core::start_beaconing() {
    // Target average: 1 heartbeat every 60 seconds
    double lambda = 1.0 / 60.0;

    while (m_running) {
        send_heartbeat();

        double delay = get_poisson_delay(lambda);

        // Use nanoseconds for precise, non-predictable sleeping
        std::this_thread::sleep_for(std::chrono::nanoseconds(static_cast<long>(delay * 1e9)));
    }
}

void Core::handle_command(const std::string& command) {
    if (command == "DIE") {
        m_running = false;
        // Trigger suicide
    }
}

bool Core::establish_quic_connection() {
    // In reality, this would involve initializing an msquic or ngtcp2 context
    return true;
}

} // namespace Phantom

int main() {
    Phantom::Core core;
    // std::cout << "[*] Core module initialized. Starting stealth beaconing..." << std::endl;
    core.start_beaconing();
    return 0;
}
