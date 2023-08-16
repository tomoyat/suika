#ifndef SUIKA_ETHERDEVICE_H
#define SUIKA_ETHERDEVICE_H

#include <string>
#include <utility>
#include <linux/if.h>
#include <cstdint>
#include <array>

namespace suika::device::ether {
    static constexpr int ETHER_ADDR_LEN = 6;

    std::string addressToString(const std::array<std::uint8_t, ETHER_ADDR_LEN> &addr);

    std::array<std::uint8_t, ETHER_ADDR_LEN> stringToAddress(const std::string &str);

    struct EtherDevice {
        int fd{};
        ifreq ifr{};
        std::string tanDevice;
        std::string tanDeviceName;
        std::array<std::uint8_t, ETHER_ADDR_LEN> address{};

        EtherDevice(std::string tanDevice_, std::string tanDeviceName_,
                    const std::string &address_) :
                tanDevice{std::move(tanDevice_)}, tanDeviceName{std::move(tanDeviceName_)} {
            address = stringToAddress(address_);
        }

        int open();

    private:
        void fetchMacAddress(std::array<std::uint8_t, ETHER_ADDR_LEN> &addr);
    };
}

#endif //SUIKA_ETHERDEVICE_H
