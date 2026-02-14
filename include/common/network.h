#ifndef PHANTOM_NETWORK_H
#define PHANTOM_NETWORK_H

#include <string>
#include <vector>
#include <netinet/in.h>

namespace Phantom {
namespace Network {

    struct QUICPacket {
        uint8_t flags;
        uint64_t connection_id;
        uint32_t packet_number;
        std::vector<uint8_t> payload;
    };

    class Transport {
    public:
        Transport(const std::string& host, uint16_t port);
        ~Transport();

        bool send_packet(const std::vector<uint8_t>& data);
        std::vector<uint8_t> receive_packet();

    private:
        int m_sock;
        struct sockaddr_in m_server_addr;
        uint64_t m_conn_id;
        uint32_t m_pkt_num;

        std::vector<uint8_t> serialize_quic(const std::vector<uint8_t>& data);
    };

}
}

#endif // PHANTOM_NETWORK_H
