#ifndef SUIKA_IPV4_H
#define SUIKA_IPV4_H

#include "Protocol.h"
#include "IpNetworkInterface.h"

namespace suika::protocol::ipv4 {

    constexpr uint8_t IP_VERSION_IPV4 = 4;

    constexpr uint8_t ICMP_TYPE = 0x01;
    constexpr uint8_t UDP_TYPE = 0x11;

    constexpr uint32_t IP_ADDR_ANY = 0x00000000; /* 0.0.0.0 */
    constexpr uint32_t IP_ADDR_BROADCAST = 0xffffffff; /* 255.255.255.255 */

    struct Ipv4ProtocolHandler : suika::protocol::ProtocolHandler {
        int handle(std::shared_ptr<suika::protocol::ProtocolData> protocolDataPtr) override;
    };

    struct Ipv4ProtocolData {
        std::uint8_t type;
        std::vector<std::uint8_t> data;
        std::uint32_t src;
        std::uint32_t dst;
        std::shared_ptr<suika::network::IpNetworkInterface> ipNetworkInterfacePtr;

    };

    struct ProtocolHandler {
        virtual int handle(std::shared_ptr<Ipv4ProtocolData> protocolDataPtr) = 0;

        virtual ~ProtocolHandler() = default;
    };
    inline std::map<std::uint8_t, std::shared_ptr<ProtocolHandler> > protocolHandlers;

    struct EventHandler {
        virtual int handle() = 0;
        virtual ~EventHandler() = default;
    };
    inline std::vector<std::shared_ptr<EventHandler> > eventHandlers;

    int ipv4_output(std::uint8_t protocol, const std::vector<std::uint8_t> &data, std::uint32_t src, std::uint32_t dst);

    std::uint16_t ip_gen_id(void);

    struct IpEndpoint {
        std::uint32_t addr;
        std::uint16_t port;
    };
}

#endif //SUIKA_IPV4_H
