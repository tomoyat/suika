#ifndef SUIKA_ICMP_H
#define SUIKA_ICMP_H

#include "Ipv4.h"

namespace suika::protocol::icmp {

    constexpr int TYPE_ECHO_REPLY = 0;
    constexpr int TYPE_ECHO = 8;

    struct IcmpHandler : suika::protocol::ipv4::ProtocolHandler {
        int handle(std::shared_ptr<suika::protocol::ipv4::Ipv4ProtocolData> protocolDataPtr) override;
    };

    int sendIcmp(
            std::uint16_t identifier,
            std::uint16_t sequenceNumber,
            std::uint32_t dst,
            const std::shared_ptr<suika::network::IpNetworkInterface> &ipNetworkInterfacePtr);
}


#endif //SUIKA_ICMP_H
