#ifndef SUIKA_ETHERDEVICE_H
#define SUIKA_ETHERDEVICE_H

#include <string>
#include <utility>
#include <linux/if.h>
#include <cstdint>
#include <array>

#include "NetworkInterface.h"
#include "Device.h"

namespace suika::ether {
    constexpr int ETHER_ADDR_LEN = 6;
    constexpr int IP_ADDR_LEN = 4;
    // https://www.iana.org/assignments/ieee-802-numbers/ieee-802-numbers.txt
    constexpr int ETHER_TYPE_IP = 0x0800;
    constexpr int ETHER_TYPE_ARP = 0x0806;

    constexpr int ETHER_FRAME_SIZE_MAX = 1500;
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
        std::shared_ptr<suika::device::Device> selfPtr;

        EtherDevice(std::string tanDevice_, std::string tanDeviceName_,
                    const std::string &address_) :
                tanDevice{std::move(tanDevice_)}, tanDeviceName{std::move(tanDeviceName_)} {
            address = stringToAddress(address_);
        }

        int open() override;

        int handler(pthread_t) override;

        int getIrq() override;

        std::string getInfo() override;

        int addNetworkInterface(std::shared_ptr<suika::network::NetworkInterface> interfacePtr) override;

        void fetchMacAddress(std::array<std::uint8_t, suika::ether::ETHER_ADDR_LEN> &addr);

        void setSelfPtr(std::shared_ptr<suika::device::Device> selfPtr_) override;

        std::vector<std::uint8_t> getAddress() override;

        std::shared_ptr<suika::device::Device> getSelfPtr() override;

        int transmit(const std::vector<uint8_t> &data, const std::vector<std::uint8_t> &dst, std::uint16_t type) override;
    };
};

#endif //SUIKA_ETHERDEVICE_H
