#ifndef SUIKA_ETHERDEVICE_H
#define SUIKA_ETHERDEVICE_H

#include <string>
#include <utility>
#include <linux/if.h>
#include <cstdint>
#include <array>

namespace suika::device {
    struct Device {
        virtual int open() = 0;

        virtual int handler(pthread_t) = 0;

        virtual int getIrq() = 0;

        virtual std::string getInfo() = 0;

        virtual ~Device() = default;
    };
};

namespace suika::ether {
    static constexpr int ETHER_ADDR_LEN = 6;
        static constexpr int IP_ADDR_LEN = 4;
    // https://www.iana.org/assignments/ieee-802-numbers/ieee-802-numbers.txt
    static constexpr int ETHER_TYPE_IP = 0x0800;
    static constexpr int ETHER_TYPE_ARP = 0x0806;
}

namespace suika::device::ether {
    std::string addressToString(const std::array<std::uint8_t, suika::ether::ETHER_ADDR_LEN> &addr);

    std::array<std::uint8_t, suika::ether::ETHER_ADDR_LEN> stringToAddress(const std::string &str);

    struct EtherDevice : suika::device::Device {
        int fd{};
        int irq{};
        ifreq ifr{};
        std::string tanDevice;
        std::string tanDeviceName;
        std::array<std::uint8_t, suika::ether::ETHER_ADDR_LEN> address{};

        EtherDevice(std::string tanDevice_, std::string tanDeviceName_,
                    const std::string &address_) :
                tanDevice{std::move(tanDevice_)}, tanDeviceName{std::move(tanDeviceName_)} {
            address = stringToAddress(address_);
        }

        int open() override;

        int handler(pthread_t) override;

        int getIrq() override;

        std::string getInfo() override;

        void fetchMacAddress(std::array<std::uint8_t, suika::ether::ETHER_ADDR_LEN> &addr);
    };
};

#endif //SUIKA_ETHERDEVICE_H
