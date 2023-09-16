#ifndef SUIKA_ARPDATA_H
#define SUIKA_ARPDATA_H

#include <vector>
#include <cstdint>
#include <utility>

#include "Protocol.h"
#include "BinaryPayload.h"

namespace suika::protocol::arp {

    constexpr int ARP_HEADER_SIZE = 8;
    constexpr int OP_REQUEST = 0x0001;
    constexpr int OP_REPLY = 0x0002;

    struct ArpData : BinaryPayload {
        explicit ArpData(const std::shared_ptr<suika::protocol::ProtocolData> &protocolData_) {
          data = std::vector<uint8_t>(protocolData_->data.size(), 0);
          std::copy(protocolData_->data.begin(), protocolData_->data.end(), data.begin());
        };

        explicit ArpData(int size) {
            data = std::vector<std::uint8_t>(size, 0);
        }

        std::uint16_t hardwareType() const {
            return readUint16(0);
        }

        void setHardwareType(uint16_t value) {
            writeUint16(value, 0);
        }

        std::uint16_t protocolType() const {
            return readUint16(2);
        }

        void setProtocolType(std::uint16_t value) {
            writeUint16(value, 2);
        }

        std::uint8_t hardwareAddressLength() const {
            return readUint8(4);
        }

        void setHardwareAddressLength(std::uint8_t value) {
            writeUint8(value, 4);
        }

        std::uint8_t protocolAddressLength() const {
            return readUint8(5);
        }

        void setProtocolAddressLength(std::uint8_t value) {
            writeUint8(value, 5);
        }

        std::uint16_t operationCode() const {
            // operation code
            // https://www.iana.org/assignments/arp-parameters/arp-parameters.xhtml
            return readUint16(6);
        }

        void setOperationCode(std::uint16_t value) {
            writeUint16(value, 6);
        }

        [[nodiscard]] std::vector<std::uint8_t> senderHardwareAddress() const {
            return getAddress(hardwareAddressLength(), ARP_HEADER_SIZE);
        }

        void setSenderHardwareAddress(const std::vector<std::uint8_t> &address) {
            writeVector(address, ARP_HEADER_SIZE);
        }

        std::vector<std::uint8_t> senderProtocolAddress() const {
            int offset = ARP_HEADER_SIZE + hardwareAddressLength();
            return getAddress(protocolAddressLength(), offset);
        }

        void setSenderProtocolAddress(const std::vector<std::uint8_t> &address) {
            writeVector(address, ARP_HEADER_SIZE + hardwareAddressLength());
        }

        std::vector<std::uint8_t> targetHardwareAddress() const {
            auto hal = hardwareAddressLength();
            int offset = ARP_HEADER_SIZE + hardwareAddressLength() + protocolAddressLength();
            return getAddress(hal, offset);
        }

        void setTargetHardwareAddress(const std::vector<std::uint8_t> &address) {
            writeVector(address, ARP_HEADER_SIZE + hardwareAddressLength() + protocolAddressLength());
        }

        std::vector<std::uint8_t> targetProtocolAddress() const {
            auto hal = hardwareAddressLength();
            auto pal = protocolAddressLength();
            int offset = ARP_HEADER_SIZE + hal * 2 + pal;
            return getAddress(pal, offset);
        }

        void setTargetProtocolAddress(const std::vector<std::uint8_t> &address) {
            writeVector(address, ARP_HEADER_SIZE + hardwareAddressLength() * 2 + protocolAddressLength());
        }

    private:
        std::vector<std::uint8_t> getAddress(int len, int offset) const {
            std::vector<std::uint8_t> ret;
            for (int i = 0; i < len; i++) {
                ret.push_back(data[i + offset]);
            }
            return ret;
        }
    };

    inline ArpData arpDataFactory(
            int hardwareType,
            int protocolType,
            int hardwareAddressLength,
            int protocolAddressLength,
            int operationCode
    ) {
        int size = ARP_HEADER_SIZE + hardwareAddressLength * 2 + protocolAddressLength * 2;
        auto d = ArpData(size);
        d.setHardwareType(hardwareType);
        d.setProtocolType(protocolType);
        d.setHardwareAddressLength(hardwareAddressLength);
        d.setProtocolAddressLength(protocolAddressLength);
        d.setOperationCode(operationCode);
        return d;
    }

    void arpDump(const ArpData &arpData);
}

#endif //SUIKA_ARPDATA_H
