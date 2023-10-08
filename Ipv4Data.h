#ifndef SUIKA_IPV4DATA_H
#define SUIKA_IPV4DATA_H

#include "BinaryPayload.h"
#include "Protocol.h"

namespace suika::protocol::ipv4 {
    struct Ipv4Data : BinaryPayload {
        explicit Ipv4Data(const std::shared_ptr<suika::protocol::ProtocolData> &protocolData_) {
            data = std::vector<uint8_t>(protocolData_->data.size(), 0);
            std::copy(protocolData_->data.begin(), protocolData_->data.end(), data.begin());
        };

        explicit Ipv4Data(int size) {
            data = std::vector<std::uint8_t>(size, 0);
        }

        [[nodiscard]] std::uint8_t versionAndHeaderLength() const {
            return readUint8(0);
        }

        void versionAndHeaderLength(std::uint8_t value) {
            writeUint8(value, 0);
        }

        [[nodiscard]] std::uint8_t version() const {
            auto value = versionAndHeaderLength();
            return static_cast<std::uint8_t>((value & 0xF0) >> 4);
        }

        [[nodiscard]] std::uint8_t headerLength() const {
            auto value = versionAndHeaderLength();
            return static_cast<std::uint8_t>(value & 0x0F);
        }

        void version(std::uint8_t value) {
            auto hl = headerLength();
            auto val = static_cast<std::uint8_t>(value << 4);
            writeUint8(value, val | hl);
        }

        // 32bitが何個分かを示す
        // byte数にするならかける4が必要
        void headerLength(std::uint8_t value) {
            auto v = version();
            writeUint8(value, (v << 4) | (value & 0x0F));
        }

        [[nodiscard]] std::uint8_t typeOfService() const {
            return readUint8(1);
        }

        void typeOfService(std::uint8_t value) {
            writeUint8(value, 1);
        }

        [[nodiscard]] std::uint16_t totalLength() const {
            return readUint16(2);
        }

        void totalLength(std::uint16_t value) {
            writeUint16(value, 2);
        }

        [[nodiscard]] std::uint16_t identification() const {
            return readUint16(4);
        }

        void identification(std::uint16_t value) {
            writeUint16(value, 4);
        }


        [[nodiscard]] std::uint16_t flagsAndFragmentOffset() const {
            return readUint16(6);
        }

        void flagsAndFragmentOffset(std::uint16_t value) {
            writeUint16(value, 6);
        }

        [[nodiscard]] std::uint8_t timeToLive() const {
            return readUint8(8);
        }

        void timeToLive(std::uint8_t value) {
            writeUint8(value, 8);
        }

        [[nodiscard]] std::uint8_t protocol() const {
            return readUint8(9);
        }

        void protocol(std::uint8_t value) {
            writeUint8(value, 9);
        }


        [[nodiscard]] std::uint16_t checksum() const {
            return readUint16(10);
        }

        void checksum(std::uint16_t value) {
            writeUint16(value, 10);
        }

        [[nodiscard]] std::uint32_t src() const {
            return readUint32(12);
        }

        void src(std::uint32_t value) {
            writeUint32(value, 12);
        }

        [[nodiscard]] std::uint32_t dst() const {
            return readUint32(16);
        }

        void dst(std::uint16_t value) {
            writeUint32(value, 16);
        }

        [[nodiscard]] bool verifyHeader() const {
            auto headerByteLen = headerLength() * 4;
            std::uint32_t sum = 0;
            for (int i = 0; i < headerByteLen && i < data.size(); i+=2) {
                sum += static_cast<std::uint32_t>(data[i]) << 8 | static_cast<std::uint32_t>(data[i+1]);
            }
            auto ret = static_cast<std::uint16_t>((sum + (sum >> 16)) & 0x0000FFFF);
            if (ret == 0xFFFF) {
                return true;
            }
            return false;
        }
    };
}
#endif //SUIKA_IPV4DATA_H
