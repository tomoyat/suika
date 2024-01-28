#ifndef SUIKA_IPV4DATA_H
#define SUIKA_IPV4DATA_H

#include "BinaryPayload.h"
#include "Protocol.h"
#include "Ipv4.h"

namespace suika::protocol::ipv4 {
    struct Ipv4Data : BinaryPayload {
        explicit Ipv4Data(const std::shared_ptr<suika::protocol::ProtocolData> &protocolData_) {
            data = std::vector<uint8_t>(protocolData_->data.size(), 0);
            std::copy(protocolData_->data.begin(), protocolData_->data.end(), data.begin());
        };

        explicit Ipv4Data(int size) {
            data = std::vector<std::uint8_t>(size, 0);
        }

        explicit Ipv4Data() {
            data = std::vector<std::uint8_t>(20, 0);
            this->version(suika::protocol::ipv4::IP_VERSION_IPV4);
            this->headerLength(5);
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

        [[nodiscard]] std::uint16_t headerLengthByte() const {
            std::uint16_t l = headerLength();
            return l * 4;
        }

        void version(std::uint8_t value) {
            auto hl = headerLength();
            auto val = static_cast<std::uint8_t>(value << 4);
            writeUint8(val | hl, 0);
        }

        // 32bitが何個分かを示す
        // byte数にするならかける4が必要
        void headerLength(std::uint8_t value) {
            auto v = version();
            writeUint8((v << 4) | (value & 0x0F), 0);
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

        void dst(std::uint32_t value) {
            writeUint32(value, 16);
        }

        std::vector<std::uint8_t> payload() {
            auto dataLength = totalLength() - headerLengthByte();
            auto ret = std::vector<std::uint8_t>(dataLength);
            std::copy(data.begin() + headerLengthByte(),
                      data.begin() + totalLength(),
                      ret.begin());
            return ret;
        }

        void payload(std::vector<uint8_t> d) {
            std::uint16_t l = headerLengthByte() + d.size();
            totalLength(l);
            data.resize(l, 0);
            std::copy(d.begin(), d.begin() + d.size(), data.begin() + headerLengthByte());
        }

        [[nodiscard]] bool verifyHeader() const {
            auto headerByteLen = headerLength() * 4;
            auto ret = suika::ipUtils::calculateChecksum(data, 0, headerByteLen);
            if (ret == 0xFFFF) {
                return true;
            }
            return false;
        }

        void setCheckSum() {
            auto headerLen = headerLengthByte();
            checksum(0);
            std::uint16_t sum = ~suika::ipUtils::calculateChecksum(data, 0, headerLen);
            checksum(sum);
        }

        [[nodiscard]] std::string info() const {
            return
            std::format("version = {}, headerLength = {}, ", version(), headerLength())
            +
            std::format("typeOfService = {}, totalLength = {}, ", typeOfService(), totalLength())
            +
            std::format("identification = {}, flagsAndFragmentOffset = {}, ", identification(), flagsAndFragmentOffset())
            +
            std::format("timeToLive = {}, protocol = {}, ", timeToLive(), protocol())
            +
            std::format("src = {}, dst = {}, ", suika::ipUtils::Uint32ToIpv4str(src()), suika::ipUtils::Uint32ToIpv4str(dst()));
        }
    };
}
#endif //SUIKA_IPV4DATA_H
