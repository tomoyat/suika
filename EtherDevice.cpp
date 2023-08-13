#include "EtherDevice.h"

#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <sys/fcntl.h>
#include <cstring>

#include "util.h"
#include "logger.h"

namespace suika::device {
    int EtherDevice::open() {
        fd = ::open(tanDevice.c_str(), O_RDWR);
        if (fd < 0) {
            suika::logger::error("error open tun device");
            throw std::runtime_error("error open tun device");
        }
        strncpy(ifr.ifr_name, tanDeviceName.c_str(), IFNAMSIZ);
        ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
        if (ioctl(fd, TUNSETIFF, &ifr) == -1) {
            close(fd);
            suika::logger::error("ioctl error. TUNSETIFF");
            throw std::runtime_error("ioctl error. TUNSETIFF");
        }
        suika::logger::info(std::format("allocated interface {}", tanDeviceName));

        /* Set Asynchronous I/O signal delivery destination */
        if (fcntl(fd, F_SETOWN, getpid()) == -1) {
            ::close(fd);
            auto msg = std::format("fcntl(F_SETOWN): {}, dev={}", strerror(errno), tanDeviceName);
            suika::logger::error(msg);
            throw std::runtime_error(msg);
        }

        /* Enable Asynchronous I/O */
        if (fcntl(fd, F_SETFL, O_ASYNC) == -1) {
            ::close(fd);
            auto msg = std::format("fcntl(F_SETFL): {}, dev={}", strerror(errno), tanDeviceName);
            suika::logger::error(msg);
            throw std::runtime_error(msg);
        }

        /* Use other signal instead of SIGIO */
        if (fcntl(fd, F_SETSIG, suika::util::ETHER_TAN_IRQ) == -1) {
            ::close(fd);
            auto msg = std::format("fcntl(F_SETSIG): {}, dev={}", strerror(errno), tanDeviceName);
            suika::logger::error(msg);
            throw std::runtime_error(msg);
        }

        // mac addressを取得する
        int soc;
        ifreq tmpIfr{};
        soc = socket(AF_INET, SOCK_DGRAM, 0);
        if (soc == -1) {
            ::close(fd);
            auto msg = std::format("socket: {}, dev={}", strerror(errno), tanDeviceName);
            suika::logger::error(msg);
            throw std::runtime_error(msg);
        }
        strncpy(tmpIfr.ifr_name, tanDeviceName.c_str(), sizeof(tmpIfr.ifr_name)-1);
        if (ioctl(soc, SIOCGIFHWADDR, &tmpIfr) == -1) {
            ::close(fd);
            auto msg = std::format("ioctl [SIOCGIFHWADDR]: {}, dev={}", strerror(errno), tanDeviceName);
            suika::logger::error(msg);
            throw std::runtime_error(msg);
        }
        memcpy(address, tmpIfr.ifr_hwaddr.sa_data, suika::device::ETHER_ADDR_LEN);
        suika::logger::info(std::format("ether device: mac address={}", address));
        ::close(soc);
        return 0;
    }
}
