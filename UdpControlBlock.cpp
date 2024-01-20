#include "UdpControlBlock.h"

namespace suika::protocol::udp::pcb {
    UdpPcbManager::UdpPcbManager() : udpPcbList{100} {
    }

    UdpPcbManager::~UdpPcbManager() = default;

    UdpControlBlock &UdpPcbManager::get(const int id_) {
        return udpPcbList.at(id_);
    }

    std::expected<std::reference_wrapper<UdpControlBlock>, int>
    UdpPcbManager::alloc() {
        for (size_t i = 0; UdpControlBlock &elm: udpPcbList) {
            if (elm.state == UdpControlBlockState::FREE) {
                elm.state = UdpControlBlockState::OPEN;
                elm.id = static_cast<int>(i);
                elm.endpoint.addr = ipv4::IP_ADDR_ANY;
                elm.endpoint.port = 0;
                return std::ref(elm);
            }
            i++;
        }
        return std::unexpected{-1};
    }

    std::expected<std::reference_wrapper<UdpControlBlock>, int>
    UdpPcbManager::select(std::uint32_t ipv4, std::uint16_t port) {
        for (size_t i = 0; UdpControlBlock &elm: udpPcbList) {
            if (elm.state == UdpControlBlockState::OPEN &&
                (elm.endpoint.addr == ipv4::IP_ADDR_ANY || elm.endpoint.addr == ipv4) &&
                elm.endpoint.port == port) {
                return std::ref(elm);
            }
            i++;
        }
        return std::unexpected{-1};
    }

    UdpPcbManager *UdpPcbManager::getInstance() {
        static UdpPcbManager instance;
        return &instance;
    }
}
