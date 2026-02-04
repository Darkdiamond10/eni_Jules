#include "common/network.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <random>

namespace Phantom {
namespace Network {

Transport::Transport(const std::string& host, uint16_t port) : m_pkt_num(0) {
    m_sock = socket(AF_INET, SOCK_DGRAM, 0);
    m_server_addr.sin_family = AF_INET;
    m_server_addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &m_server_addr.sin_addr);

    // If binding fails, it's a client. If it succeeds, it's a server.
    // This is a bit hacky but works for this prototype.
    int opt = 1;
    setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    bind(m_sock, (struct sockaddr*)&m_server_addr, sizeof(m_server_addr));

    std::random_device rd;
    m_conn_id = ((uint64_t)rd() << 32) | rd();
}

Transport::~Transport() {
    if (m_sock >= 0) close(m_sock);
}

std::vector<uint8_t> Transport::serialize_quic(const std::vector<uint8_t>& data) {
    std::vector<uint8_t> pkt;
    // Short Header: 010xxxxx (Flags)
    pkt.push_back(0x40);

    // Connection ID
    for (int i = 0; i < 8; ++i) {
        pkt.push_back((m_conn_id >> (i * 8)) & 0xFF);
    }

    // Packet Number
    pkt.push_back(m_pkt_num & 0xFF);
    m_pkt_num++;

    // Payload
    pkt.insert(pkt.end(), data.begin(), data.end());

    return pkt;
}

bool Transport::send_packet(const std::vector<uint8_t>& data) {
    std::vector<uint8_t> quic_data = serialize_quic(data);

    // For the server, we need to send back to whoever sent to us.
    // For this prototype, we'll use the cached m_server_addr.
    ssize_t sent = sendto(m_sock, quic_data.data(), quic_data.size(), 0,
                          (struct sockaddr*)&m_server_addr, sizeof(m_server_addr));
    return sent == (ssize_t)quic_data.size();
}

std::vector<uint8_t> Transport::receive_packet() {
    uint8_t buffer[2048];
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    setsockopt(m_sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

    socklen_t addr_len = sizeof(m_server_addr);
    ssize_t recvd = recvfrom(m_sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&m_server_addr, &addr_len);
    if (recvd <= 10) return {}; // Too small for QUIC header

    // Strip header (simplified for prototype)
    std::vector<uint8_t> data(buffer + 10, buffer + recvd);
    return data;
}

}
}
