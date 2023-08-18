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
        virtual int handler() = 0;
        virtual int getIrq() = 0;
        virtual std::string getInfo() = 0;
        virtual ~Device() {};
    };
}

namespace suika::device::ether {
    static constexpr int ETHER_ADDR_LEN = 6;

    std::string addressToString(const std::array<std::uint8_t, ETHER_ADDR_LEN> &addr);

    std::array<std::uint8_t, ETHER_ADDR_LEN> stringToAddress(const std::string &str);

    struct EtherDevice : suika::device::Device {
        int fd{};
        int irq;
        ifreq ifr{};
        std::string tanDevice;
        std::string tanDeviceName;
        std::array<std::uint8_t, ETHER_ADDR_LEN> address{};

        EtherDevice(std::string tanDevice_, std::string tanDeviceName_,
                    const std::string &address_) :
                tanDevice{std::move(tanDevice_)}, tanDeviceName{std::move(tanDeviceName_)} {
            address = stringToAddress(address_);
        }

        int open() override;
        int handler() override;
        int getIrq() override;
        std::string getInfo() override;

    private:
        void fetchMacAddress(std::array<std::uint8_t, ETHER_ADDR_LEN> &addr);
    };
}

#endif //SUIKA_ETHERDEVICE_H
