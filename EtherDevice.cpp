#include "EtherDevice.h"

#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <sys/fcntl.h>
#include <cstring>
#include <ranges>

#include "Util.h"
#include "Logger.h"
#include "Protocol.h"

namespace suika::device::ether {
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
        return 0;
    }

    int EtherDevice::handler(pthread_t pthread) {
        char buffer[1514];
        // https://www.gabriel.urdhr.fr/2021/05/08/tuntap/
        // readは1frameだけを取れるらしい？
        // signalが抜けるかもしれないので、poolとかで読めるだけ読むのが良い
        int numOfRead = read(fd, buffer, 1514);

        std::array<std::uint8_t, suika::ether::ETHER_ADDR_LEN> srcAddr;
        std::array<std::uint8_t, suika::ether::ETHER_ADDR_LEN> dstAddr;

        for (int i = 0, cnt = 0; i < numOfRead && cnt < suika::ether::ETHER_ADDR_LEN; i++, cnt++) {
            dstAddr[cnt] = static_cast<std::uint8_t>(buffer[i]);
        }

        for (int i = 6, cnt = 0; i < numOfRead && cnt < suika::ether::ETHER_ADDR_LEN; i++, cnt++) {
            srcAddr[cnt] = static_cast<std::uint8_t>(buffer[i]);
        }

        // protocol type: https://www.iana.org/assignments/ieee-802-numbers/ieee-802-numbers.xhtml
        std::uint16_t etherType = 0;
        etherType = static_cast<std::uint8_t>(buffer[12]);
        etherType = etherType << 8;
        etherType = etherType | static_cast<std::uint8_t>(buffer[13]);

        std::vector<std::byte> data{};
        data.reserve(numOfRead);
        for (int i = 14; i < numOfRead; i++) {
            data.push_back(static_cast<std::byte>(buffer[i]));
        }

        std::lock_guard<std::mutex> lock(suika::protocol::protocolQueuesMutex);
        if (suika::protocol::protocolQueues.find(etherType) == suika::protocol::protocolQueues.end()) {
            suika::logger::warn(std::format("ether deivce protocol queues not found address = {}",
                                            static_cast<void *>(&suika::protocol::protocolQueues[suika::protocol::arpType])));
            return 0;
        }
        suika::protocol::protocolQueues[etherType].push(
                std::make_shared<suika::protocol::ProtocolData>(
                        suika::protocol::ProtocolData{etherType, data, getSelfPtr()}
                )
        );

        suika::logger::debug(
                std::format("ether frame: src={}, dst={}, type={:04x}", addressToString(srcAddr),
                            addressToString(dstAddr), etherType));

        pthread_kill(pthread, SIGUSR1);
        return 0;
    }

    int EtherDevice::getIrq() {
        return suika::util::ETHER_TAN_IRQ;
    }

    std::string EtherDevice::getInfo() {
        return std::format("EtherDevice: address = {}", addressToString(address));
    }

    void EtherDevice::fetchMacAddress(std::array<std::uint8_t, suika::ether::ETHER_ADDR_LEN> &addr) {
        int soc;
        ifreq tmpIfr{};
        soc = socket(AF_INET, SOCK_DGRAM, 0);
        if (soc == -1) {
            ::close(fd);
            auto msg = std::format("socket: {}, dev={}", strerror(errno), tanDeviceName);
            suika::logger::error(msg);
            throw std::runtime_error(msg);
        }
        strncpy(tmpIfr.ifr_name, tanDeviceName.c_str(), sizeof(tmpIfr.ifr_name) - 1);
        if (ioctl(soc, SIOCGIFHWADDR, &tmpIfr) == -1) {
            ::close(fd);
            auto msg = std::format("ioctl [SIOCGIFHWADDR]: {}, dev={}", strerror(errno), tanDeviceName);
            suika::logger::error(msg);
            throw std::runtime_error(msg);
        }
        for (int i = 0; i < suika::ether::ETHER_ADDR_LEN; i++) {
            addr[i] = static_cast<std::uint8_t>(tmpIfr.ifr_hwaddr.sa_data[i]);
        }
        suika::logger::info(std::format("ether device: mac address={}", addressToString(addr)));
        ::close(soc);
    }

    int EtherDevice::addNetworkInterface(std::shared_ptr<suika::network::NetworkInterface> interfacePtr) {
        // TODO check type
        networkInterfaces.push_back(interfacePtr);
        return 0;
    }

    std::shared_ptr<suika::device::Device> EtherDevice::getSelfPtr() {
        return selfPtr;
    }

    void EtherDevice::setSelfPtr(std::shared_ptr<suika::device::Device> selfPtr_) {
        selfPtr = selfPtr_;
    }

    std::vector<std::uint8_t> EtherDevice::getAddress() {
        std::vector<std::uint8_t> ret;
        ret.reserve(suika::ether::ETHER_ADDR_LEN);
        for (const auto& v : address) {
            ret.push_back(v);
        }
        return ret;
    }

    std::string addressToString(const std::array<std::uint8_t, suika::ether::ETHER_ADDR_LEN> &addr) {
        return std::format("{:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}", addr[0], addr[1], addr[2], addr[3], addr[4],
                           addr[5]);
    }

    std::array<std::uint8_t, suika::ether::ETHER_ADDR_LEN> stringToAddress(const std::string &str) {
        auto ar = std::array<std::uint8_t, suika::ether::ETHER_ADDR_LEN>{};
        int idx = 0;
        for (auto word: str | std::views::split(':')) {
            int value;
            std::from_chars(word.data(), word.data() + word.size(), value, 16);
            if (idx >= suika::ether::ETHER_ADDR_LEN) {
                throw std::runtime_error(std::format("invalid format {}", str));
            }
            ar[idx] = static_cast<uint8_t>(value);
            idx++;
        }
        return ar;
    }
}
