#ifndef SUIKA_ARP_H
#define SUIKA_ARP_H

#include <utility>
#include <cstdint>
#include <string>
#include <format>

#include "Protocol.h"
#include "ArpCache.h"
#include "ArpData.h"
#include "IpNetworkInterface.h"

namespace suika::protocol::arp {
    constexpr int ARP_HARDWARE_TYPE_ETHER = 0x0001;
    constexpr int ARP_PROTOCOL_TYPE_IP = 0x0800;

    constexpr int ETHER_ADDRESS_LEN = 6;
    constexpr int IP_ADDRESS_LEN = 4;

    struct ArpProtocolHandler : suika::protocol::ProtocolHandler {
        int handle(std::shared_ptr<suika::protocol::ProtocolData> protocolDataPtr) override;
    };

    std::vector<std::uint8_t> arpResolve(std::shared_ptr<suika::network::IpNetworkInterface> interface, std::uint32_t ip);

    int arpRequest(std::shared_ptr<suika::network::IpNetworkInterface> interface, std::uint32_t ip);

    std::string macAddressToString(const std::vector<uint8_t> &addr);

    std::string ipV4ToString(const std::vector<uint8_t> &addr);
}


#endif //SUIKA_ARP_H
