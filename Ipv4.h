#ifndef SUIKA_IPV4_H
#define SUIKA_IPV4_H

#include "Protocol.h"

namespace suika::protocol::ipv4 {

    constexpr uint8_t ICMP_TYPE = 0x01;

    struct Ipv4ProtocolHandler : suika::protocol::ProtocolHandler {
        int handle(std::shared_ptr<suika::protocol::ProtocolData> protocolDataPtr) override;
    };

    struct Ipv4ProtocolData {
        std::uint8_t type;
        std::vector<std::uint8_t> data;
    };

    struct ProtocolHandler {
        virtual int handle(std::shared_ptr<Ipv4ProtocolData> protocolDataPtr) = 0;
        virtual ~ProtocolHandler() = default;
    };
    inline std::map<std::uint8_t, std::shared_ptr<ProtocolHandler> > protocolHandlers;
}

#endif //SUIKA_IPV4_H
