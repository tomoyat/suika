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
#include "IpUtils.h"

namespace suika::network {
    struct IpNetworkInterface : NetworkInterface {
        uint32_t unicast{};
        uint32_t broadcast{};
        uint32_t netmask{};

        explicit IpNetworkInterface(int family_, const std::string &unicast_, const std::string &netmask_) {
            family = family_;
            unicast = suika::ipUtils::Ipv4strToUint32(unicast_);
            netmask = suika::ipUtils::Ipv4strToUint32(netmask_);
            broadcast = (unicast & netmask) | ~netmask;
        }

        std::string info() {
            return std::format(
                    "family = {}, unicast => {}, netmask = {}, broadcast = {}",
                    family,
                    suika::ipUtils::Uint32ToIpv4str(unicast),
                    suika::ipUtils::Uint32ToIpv4str(netmask),
                    suika::ipUtils::Uint32ToIpv4str(broadcast)
            );
        }

        std::shared_ptr<suika::device::Device> devicePtr{};

        int registerDevice(std::shared_ptr<suika::device::Device> ptr) override;

        std::vector<std::uint8_t> getUnicastVector() {
            std::vector<std::uint8_t> ret;
            ret.reserve(4);
            ret.push_back(static_cast<std::uint8_t>(unicast >> 24));
            ret.push_back(static_cast<std::uint8_t>(unicast >> 16));
            ret.push_back(static_cast<std::uint8_t>(unicast >> 8));
            ret.push_back(static_cast<std::uint8_t>(unicast));
            return ret;
        }
    };
}
#endif //SUIKA_IPNETWORKINTERFACE_H
