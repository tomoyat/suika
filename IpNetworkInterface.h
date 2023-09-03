#ifndef SUIKA_IPNETWORKINTERFACE_H
#define SUIKA_IPNETWORKINTERFACE_H

#include <vector>
#include <string>
#include <string_view>
#include <ranges>
#include <charconv>
#include <format>
#include <cstdint>

#include "NetworkInterface.h"

namespace suika::network {
    inline uint32_t Ipv4strToUint32(const std::string &str) {
        uint32_t ret = 0;
        int idx = 0;
        for (auto word: str | std::views::split('.')) {
            uint32_t value;
            std::from_chars(word.data(), word.data() + word.size(), value, 10);
            if (idx >= 4) {
                throw std::runtime_error(std::format("invalid format {}", str));
            }
            ret = ret << 8 | value;
            idx++;
        }
        return ret;
    }

    inline std::string Uint32ToIpv4str(std::uint32_t addr) {
        return std::format("{}.{}.{}.{}",
                           (addr >> 24) & 255, (addr >> 16) & 255,
                           (addr >> 8) & 255, addr & 255);
    }
}

namespace suika::network {
    struct IpNetworkInterface : NetworkInterface {
        uint32_t unicast{};
        uint32_t broadcast{};
        uint32_t netmask{};

        explicit IpNetworkInterface(int family_, const std::string &unicast_, const std::string &netmask_) {
            family = family_;
            unicast = Ipv4strToUint32(unicast_);
            netmask = Ipv4strToUint32(netmask_);
            broadcast = (unicast & netmask) | ~netmask;
        }

        std::string info() {
            return std::format("family = {}, unicast => {}, netmask = {}, broadcast = {}",
                               family, Uint32ToIpv4str(unicast), Uint32ToIpv4str(netmask), Uint32ToIpv4str(broadcast));
        }

        std::shared_ptr<suika::device::Device> devicePtr{};

        int registerDevice(std::shared_ptr<suika::device::Device> ptr) override;
    };


}
#endif //SUIKA_IPNETWORKINTERFACE_H
