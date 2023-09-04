#ifndef SUIKA_ARP_H
#define SUIKA_ARP_H

#include <utility>
#include <cstdint>
#include <vector>
#include <string>
#include <format>

#include "Protocol.h"

namespace suika::protocol::arp {
    constexpr int ARP_HARDWARE_TYPE_ETHER = 0x0001;

    struct ArpProtocolHandler : suika::protocol::ProtocolHandler {
        int handle(std::shared_ptr<suika::protocol::ProtocolData> protocolDataPtr) override;
    };

    std::string macAddressToString(const std::vector<uint8_t> &addr);

    std::string ipV4ToString(const std::vector<uint8_t> &addr);

    struct ArpData {
        std::vector<std::byte> data;

        explicit ArpData(const std::shared_ptr<suika::protocol::ProtocolData>& protocolData_) : data{std::move(protocolData_->data)} {};

        std::uint16_t hardwareType() {
            return (static_cast<std::uint16_t>(data[0]) << 8) | (static_cast<std::uint16_t>(data[1]));
        }

        std::uint16_t protocolType() {
            return (static_cast<std::uint16_t>(data[2]) << 8) | (static_cast<std::uint16_t>(data[3]));
        }

        std::uint8_t hardwareAddressLength() {
            return static_cast<std::uint8_t>(data[4]);
        }

        std::uint8_t protocolAddressLength() {
            return static_cast<std::uint8_t>(data[5]);
        }

        std::uint16_t operationCode() {
            // operation code
            // https://www.iana.org/assignments/arp-parameters/arp-parameters.xhtml
            return (static_cast<std::uint16_t>(data[6]) << 8) | (static_cast<std::uint16_t>(data[7]));
        }

        std::vector<std::uint8_t> senderHardwareAddress() {
            return getAddress(hardwareAddressLength(), 8);
        }

        std::vector<std::uint8_t> senderProtocolAddress() {
            int offset = 8 + hardwareAddressLength();
            return getAddress(protocolAddressLength(), offset);
        }

        std::vector<std::uint8_t> targetHardwareAddress() {
            auto hal = hardwareAddressLength();
            int offset = 8 + hardwareAddressLength() + protocolAddressLength();
            return getAddress(hal, offset);
        }

        std::vector<std::uint8_t> targetProtocolAddress() {
            auto hal = hardwareAddressLength();
            auto pal = protocolAddressLength();
            int offset = 8 + hal * 2 + pal;
            return getAddress(pal, offset);
        }

    private:
        std::vector<std::uint8_t> getAddress(int len, int offset) {
            std::vector<std::uint8_t> ret;
            for (int i = 0; i < len; i++) {
                ret.push_back(static_cast<std::uint8_t>(data[i + offset]));
            }
            return ret;
        }
    };
}


#endif //SUIKA_ARP_H
