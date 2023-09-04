#ifndef SUIKA_ARP_H
#define SUIKA_ARP_H

#include <utility>
#include <cstdint>
#include <string>
#include <format>

#include "Protocol.h"
#include "ArpData.h"

namespace suika::protocol::arp {
    constexpr int ARP_HARDWARE_TYPE_ETHER = 0x0001;

    struct ArpProtocolHandler : suika::protocol::ProtocolHandler {
        int handle(std::shared_ptr<suika::protocol::ProtocolData> protocolDataPtr) override;
    };

    std::string macAddressToString(const std::vector<uint8_t> &addr);

    std::string ipV4ToString(const std::vector<uint8_t> &addr);
}


#endif //SUIKA_ARP_H
