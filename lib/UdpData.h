#ifndef SUIKA_UDPDATA_H
#define SUIKA_UDPDATA_H

#include "BinaryPayload.h"

namespace suika::protocol::udp {
    struct UdpData : BinaryPayload {
        static constexpr int headerSize = 8; // 8byte

        explicit UdpData(const std::vector<std::uint8_t> &data_) {
            data = std::vector<uint8_t>(data_.size());
            std::copy(data_.begin(), data_.end(), data.begin());
        };

        explicit UdpData(int size) {
            data = std::vector<std::uint8_t>(size, 0);
        }

        UdpData() {
            data = std::vector<std::uint8_t>(headerSize, 0);
        }

        [[nodiscard]] std::uint16_t srcPort() const {
            return readUint16(0);
        }

        void srcPort(std::uint16_t value) {
            writeUint16(value, 0);
        }

        [[nodiscard]] std::uint16_t dstPort() const {
            return readUint16(2);
        }

        void dstPort(std::uint16_t value) {
            writeUint16(value, 2);
        }

        [[nodiscard]] std::uint16_t length() const {
            return readUint16(4);
        }

        void length(std::uint16_t value) {
            writeUint16(value, 4);
        }

        [[nodiscard]] std::uint16_t checksum() const {
            return readUint16(6);
        }

        void checksum(std::uint16_t value) {
            writeUint16(value, 6);
        }

        std::vector<std::uint8_t> payload() {
            auto ret = std::vector<std::uint8_t>(length());
            std::copy(data.begin() + headerSize,
                      data.end(),
                      ret.begin());
            return ret;
        }

        void payload(std::vector<uint8_t> d) {
            std::uint16_t l = headerSize + d.size();
            data.resize(l, 0);
            std::copy(d.begin(), d.begin() + d.size(), data.begin() + headerSize);
        }

        [[nodiscard]] std::string info() const {
            return std::format("srcPort = {}, destPort = {}, length = {}, checksum = {}",
                                 srcPort(), dstPort(), length(), checksum());
        }
    };

    struct PseudoHeader : BinaryPayload {
        static constexpr int headerSize = 12; // 12 byte

        explicit PseudoHeader() {
            data = std::vector<std::uint8_t>(headerSize, 0);
        }

        [[nodiscard]] std::uint32_t srcIp() const {
            return readUint32(0);
        }

        void srcIp(std::uint32_t value) {
            writeUint32(value, 0);
        }

        [[nodiscard]] std::uint32_t destIp() const {
            return readUint32(4);
        }

        void destIp(std::uint32_t value) {
            writeUint32(value, 4);
        }

        [[nodiscard]] std::uint8_t zero() const {
            return readUint8(8);
        }

        [[nodiscard]] std::uint8_t protocol() const {
            return readUint8(9);
        }

        void protocol(std::uint8_t value) {
            writeUint8(value, 9);
        }

        [[nodiscard]] std::uint16_t length() const {
            return readUint16(10);
        }

        void length(std::uint16_t value) {
            writeUint16(value, 10);
        }

        [[nodiscard]] std::string info() const {
            return std::format("srcIp = {}, destIp = {}, zero = {}, protocol = {}, length = {}",
                                 ipUtils::Uint32ToIpv4str(srcIp()),  ipUtils::Uint32ToIpv4str(destIp()),
                                 zero(), protocol(), length());
        }
    };
}

#endif //SUIKA_UDPDATA_H
