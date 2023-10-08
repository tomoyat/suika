#ifndef SUIKA_ICMPDATA_H
#define SUIKA_ICMPDATA_H

#include "BinaryPayload.h"
#include "format"

namespace suika::protocol::icmp {
    struct IcmpData : BinaryPayload {
        explicit IcmpData(const std::vector<std::uint8_t> &data_) {
            data = std::vector<uint8_t>(data_.size());
            std::copy(data_.begin(), data_.end(), data.begin());
        };

        explicit IcmpData(int size) {
            data = std::vector<std::uint8_t>(size, 0);
        }

        [[nodiscard]] std::uint8_t type() const {
            return readUint8(0);
        }

        void type(std::uint8_t value) {
            writeUint8(value, 0);
        }

        [[nodiscard]] std::uint8_t code() const {
            return readUint8(1);
        }

        void code(std::uint8_t value) {
            writeUint8(value, 1);
        }

        [[nodiscard]] std::uint16_t checksum() const {
            return readUint16(2);
        }

        void checksum(std::uint16_t value) {
            writeUint16(value, 2);
        }
    };

    struct IcmpDataEcho : IcmpData {
        explicit IcmpDataEcho(const std::vector<std::uint8_t> &data_) : IcmpData(data_) {}

        [[nodiscard]] std::uint16_t identifier() const {
            return readUint16(4);
        }

        void identifier(std::uint16_t value) {
            writeUint16(value, 4);
        }

        [[nodiscard]] std::uint16_t sequenceNumber() const {
            return readUint16(6);
        }

        void sequenceNumber(std::uint16_t value) {
            writeUint16(value, 6);
        }

        [[nodiscard]] std::string info() const {
            return std::format("type = {}, code = {}, checksum = {}, identifier = {}, sequenceNumber = {}",
                                 type(), code(), checksum(), identifier(), sequenceNumber());
        }
    };
}
#endif //SUIKA_ICMPDATA_H
