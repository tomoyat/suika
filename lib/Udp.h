#ifndef SUIKA_UDP_H
#define SUIKA_UDP_H

#include <vector>
#include <cstdint>
#include <string>
#include <expected>


#include "Ipv4.h"

namespace suika::protocol::udp {
    struct UdpHandler : suika::protocol::ipv4::ProtocolHandler {
        int handle(std::shared_ptr<suika::protocol::ipv4::Ipv4ProtocolData> protocolDataPtr) override;
    };

    struct UdpEventHandler : suika::protocol::ipv4::EventHandler {
        int handle() override;
    };

    /* see https://tools.ietf.org/html/rfc6335 */
    constexpr std::uint16_t SOURCE_PORT_MIN = 39584;
    constexpr std::uint16_t SOURCE_PORT_MAX = 65535;


    class UdpSocket {
        int id;

        UdpSocket(int id_, int pcbId_);

    public:
        int sendTo(const std::vector<std::uint8_t> &payload, std::string ipv4str, std::uint32_t port);

        std::expected<int, int> bind(const std::string &ipv4str, std::uint16_t port);

        int recvFrom(std::vector<std::uint8_t> &buf, std::uint32_t &ipv4, std::uint16_t &port);

        static std::expected<UdpSocket, int> open();

        int pcbId;
    };

    int udp_output(const suika::protocol::ipv4::IpEndpoint &src,
                   const suika::protocol::ipv4::IpEndpoint &dest,
                   const std::vector<std::uint8_t> &data);
}

#endif //SUIKA_UDP_H
