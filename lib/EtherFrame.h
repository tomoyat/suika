#ifndef SUIKA_ETHERFRAME_H
#define SUIKA_ETHERFRAME_H

#include <vector>
#include <cstdint>
#include <array>
#include <string>
#include <numeric>

#include "BinaryPayload.h"
#include "EtherDevice.h"

namespace suika::ether {

    struct EtherFrame : BinaryPayload {
        static constexpr int headerSize = ETHER_ADDR_LEN * 2 + 2;
        static constexpr int frameMaxSize = 1514;
        static constexpr int frameMinSize = 60;
        static constexpr int payloadMaxSize = frameMaxSize - headerSize;
        static constexpr int payloadMinSize = frameMinSize - headerSize;

        EtherFrame(std::vector<std::uint8_t> data_) {
            data = std::vector<uint8_t>(data_.size(), 0);
            std::copy(data_.begin(), data_.end(), data.begin());
        }

        EtherFrame() {
            data = std::vector<std::uint8_t>(headerSize, 0);
        }

        std::array<std::uint8_t, suika::ether::ETHER_ADDR_LEN> dstAddr() {
            return readEtherAddr(0);
        }

        void setDstAddr(const std::vector<std::uint8_t> &addr) {
            writeEtherAddr(addr, 0);
        }

        std::array<std::uint8_t, suika::ether::ETHER_ADDR_LEN> srcAddr() {
            return readEtherAddr(6);
        }

        void setSrcAddr(const std::vector<std::uint8_t> &addr) {
            writeEtherAddr(addr, 6);
        }

        void setSrcAddr(const std::array<std::uint8_t, suika::ether::ETHER_ADDR_LEN> &addr) {
            std::vector<std::uint8_t> addrVec(suika::ether::ETHER_ADDR_LEN, 0);
            std::copy(addr.begin(), addr.end(), addrVec.begin());
            writeEtherAddr(addrVec, 6);
        }

        // protocol type: https://www.iana.org/assignments/ieee-802-numbers/ieee-802-numbers.xhtml
        std::uint16_t etherType() {
            return readUint16(12);
        }

        void setEtherType(std::uint16_t type) {
            writeUint16(type, 12);
        }

        std::vector<std::uint8_t> body() {
            int len = static_cast<int>(data.size()) - 14;
            return readVector(14, len);
        }

        void setBody(const std::vector<std::uint8_t> &body) {
            data.resize(body.size() + headerSize, 0);
            writeVector(body, headerSize);

            if (data.size() < frameMinSize) {
                data.resize(frameMinSize, 0);
            }
        }

        std::string info() {
            auto src = srcAddr();
            auto dst = dstAddr();
            auto type = etherType();

            std::string srcAddrStr = std::accumulate(src.begin(), src.end(), std::string(""), [](std::string acc, std::uint8_t x) {
                if (acc.size() > 0) {
                    return std::format("{}:{:02x}", acc, x);
                }
                return std::format("{:02x}", x);
            });

            auto dstAddrStr = std::accumulate(dst.begin(), dst.end(), std::string(""), [](std::string acc, std::uint8_t x) {
                if (acc.size() > 0) {
                    return std::format("{}:{:02x}", acc, x);
                }
                return std::format("{:02x}", x);
            });

            return std::format("src={}, dst={}, type={:04x}, size={}", srcAddrStr, dstAddrStr, type, data.size());
        }

    private:
        [[nodiscard]] std::array<std::uint8_t, suika::ether::ETHER_ADDR_LEN> readEtherAddr(int offset) const {
            std::array<uint8_t, suika::ether::ETHER_ADDR_LEN> ret{};
            for (int i = 0; i < suika::ether::ETHER_ADDR_LEN; i++) {
                ret[i] = data[offset + i];
            }
            return ret;
        }

        void writeEtherAddr(const std::vector<std::uint8_t> &addr, int offset) {
            if (addr.size() != ETHER_ADDR_LEN) {
                throw std::runtime_error("invalid ether address length");
            }
            if (data.size() < offset + ETHER_ADDR_LEN) {
                data.resize(offset + ETHER_ADDR_LEN, 0);
            }
            writeVector(addr, offset);
        }

        std::string addressToString(const std::array<std::uint8_t, suika::ether::ETHER_ADDR_LEN> &addr) {
            return std::format("{:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}", addr[0], addr[1], addr[2], addr[3], addr[4],
                               addr[5]);
        }
    };
}

#endif //SUIKA_ETHERFRAME_H
