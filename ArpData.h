#ifndef SUIKA_ARPDATA_H
#define SUIKA_ARPDATA_H

#include <vector>
#include <cstdint>
#include <utility>

#include "Protocol.h"

namespace suika::protocol::arp {

    constexpr int ARP_HEADER_SIZE = 8;
    constexpr int OP_REQUEST = 0x0001;
    constexpr int OP_REPLY = 0x0002;

    struct ArpData {
        std::vector<std::byte> data;

        explicit ArpData(const std::shared_ptr<suika::protocol::ProtocolData> &protocolData_) : data{
                std::move(protocolData_->data)} {};

        explicit ArpData(int size) : data(size) {};

        std::uint16_t hardwareType() {
            return (static_cast<std::uint16_t>(data[0]) << 8) | (static_cast<std::uint16_t>(data[1]));
        }

        void setHardwareType(uint16_t value) {
            writeUint16(value, 0);
        }

        std::uint16_t protocolType() {
            return (static_cast<std::uint16_t>(data[2]) << 8) | (static_cast<std::uint16_t>(data[3]));
        }

        void setProtocolType(std::uint16_t value) {
            writeUint16(value, 2);
        }

        std::uint8_t hardwareAddressLength() {
            return static_cast<std::uint8_t>(data[4]);
        }

        void setHardwareAddressLength(std::uint8_t value) {
            writeUint8(value, 4);
        }

        std::uint8_t protocolAddressLength() {
            return static_cast<std::uint8_t>(data[5]);
        }

        void setProtocolAddressLength(std::uint8_t value) {
            writeUint8(value, 5);
        }

        std::uint16_t operationCode() {
            // operation code
            // https://www.iana.org/assignments/arp-parameters/arp-parameters.xhtml
            return (static_cast<std::uint16_t>(data[6]) << 8) | (static_cast<std::uint16_t>(data[7]));
        }

        void setOperationCode(std::uint16_t value) {
            writeUint16(value, 6);
        }

        std::vector<std::uint8_t> senderHardwareAddress() {
            return getAddress(hardwareAddressLength(), ARP_HEADER_SIZE);
        }

        void setSenderHardwareAddress(const std::vector<std::uint8_t> &address) {
            writeVector(address, ARP_HEADER_SIZE);
        }

        std::vector<std::uint8_t> senderProtocolAddress() {
            int offset = ARP_HEADER_SIZE + hardwareAddressLength();
            return getAddress(protocolAddressLength(), offset);
        }

        void setSenderProtocolAddress(const std::vector<std::uint8_t> &address) {
            writeVector(address, ARP_HEADER_SIZE + hardwareAddressLength());
        }

        std::vector<std::uint8_t> targetHardwareAddress() {
            auto hal = hardwareAddressLength();
            int offset = ARP_HEADER_SIZE + hardwareAddressLength() + protocolAddressLength();
            return getAddress(hal, offset);
        }

        void setTargetHardwareAddress(const std::vector<std::uint8_t> &address) {
            writeVector(address, ARP_HEADER_SIZE + hardwareAddressLength() + protocolAddressLength());
        }

        std::vector<std::uint8_t> targetProtocolAddress() {
            auto hal = hardwareAddressLength();
            auto pal = protocolAddressLength();
            int offset = ARP_HEADER_SIZE + hal * 2 + pal;
            return getAddress(pal, offset);
        }

        void setTargetProtocolAddress(const std::vector<std::uint8_t> &address) {
            writeVector(address, ARP_HEADER_SIZE + hardwareAddressLength() * 2 + protocolAddressLength());
        }

    private:
        std::vector<std::uint8_t> getAddress(int len, int offset) {
            std::vector<std::uint8_t> ret;
            for (int i = 0; i < len; i++) {
                ret.push_back(static_cast<std::uint8_t>(data[i + offset]));
            }
            return ret;
        }

        void writeUint8(std::uint8_t  value, int offset) {
            write(static_cast<std::byte>(value), offset);
        }

        void writeUint16(std::uint16_t value, int offset) {
            write(static_cast<std::byte>(value >> 8), offset);
            write(static_cast<std::byte>(value & 8), offset + 1);
        }

        void writeVector(const std::vector<std::uint8_t > &vec, int offset) {
            for (int idx = 0; auto &v : vec) {
                data[offset + idx] = static_cast<std::byte>(v);
                idx++;
            }
        }

        void write(std::byte b, int offset) {
            data[offset] = b;
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
}

#endif //SUIKA_ARPDATA_H
