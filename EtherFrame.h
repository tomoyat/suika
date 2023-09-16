#ifndef SUIKA_ETHERFRAME_H
#define SUIKA_ETHERFRAME_H

#include <vector>
#include <cstdint>
#include <array>
#include "BinaryPayload.h"
#include "EtherDevice.h"

namespace suika::ether {

    struct EtherFrame : BinaryPayload {
        EtherFrame(std::vector<std::uint8_t> data_) {
            data = std::vector<uint8_t>(data_.size(), 0);
            std::copy(data_.begin(), data_.end(), data.begin());
        }

        std::array<std::uint8_t, suika::ether::ETHER_ADDR_LEN> dstAddr() {
            return readEtherAddr(0);
        }

        std::array<std::uint8_t, suika::ether::ETHER_ADDR_LEN> srcAddr() {
            return readEtherAddr(6);
        }
        // protocol type: https://www.iana.org/assignments/ieee-802-numbers/ieee-802-numbers.xhtml
        std::uint16_t etherType() {
            return readUint16(12);
        }

        std::vector<uint8_t> body() {
            int len = static_cast<int>(data.size()) - 14;
            return readVector(14, len);
        }
    private:
        std::array<std::uint8_t, suika::ether::ETHER_ADDR_LEN> readEtherAddr(int offset) {
            std::array<uint8_t, suika::ether::ETHER_ADDR_LEN> ret{};
            for (int i = 0; i < suika::ether::ETHER_ADDR_LEN; i++) {
                ret[i] = data[offset + i];
            }
            return ret;
        }
    };
}

#endif //SUIKA_ETHERFRAME_H
