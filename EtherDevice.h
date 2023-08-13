#ifndef SUIKA_ETHERDEVICE_H
#define SUIKA_ETHERDEVICE_H

#include <string>
#include <utility>
#include <linux/if.h>

namespace suika::device {
    static constexpr int ETHER_ADDR_LEN = 16;

    struct EtherDevice {
        int fd{};
        ifreq ifr{};
        std::string tanDevice;
        std::string tanDeviceName;
        char address[ETHER_ADDR_LEN];

        EtherDevice(std::string tanDevice_, std::string tanDeviceName_) :
                tanDevice{std::move(tanDevice_)}, tanDeviceName{std::move(tanDeviceName_)} {}

        int open();
    };
}

#endif //SUIKA_ETHERDEVICE_H
