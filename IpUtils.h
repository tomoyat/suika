#ifndef SUIKA_IPUTILS_H
#define SUIKA_IPUTILS_H

#include <string>
#include <string_view>
#include <ranges>
#include <charconv>
#include <format>
#include <cstdint>

namespace suika::ipUtils {
    inline uint32_t Ipv4strToUint32(const std::string &str) {
        uint32_t ret = 0;
        int idx = 0;
        for (auto word: str | std::views::split('.')) {
            uint32_t value;
            std::from_chars(word.data(), word.data() + word.size(), value, 10);
            if (idx >= 4) {
                throw std::runtime_error(std::format("invalid format {}", str));
            }
            ret = ret << 8 | value;
            idx++;
        }
        return ret;
    }

    inline std::string Uint32ToIpv4str(std::uint32_t addr) {
        return std::format("{}.{}.{}.{}",
                           (addr >> 24) & 255, (addr >> 16) & 255,
                           (addr >> 8) & 255, addr & 255);
    }

    inline std::vector<std::uint8_t> Uint32ToVector(std::uint32_t addr) {
        return std::vector<std::uint8_t>{
            static_cast<std::uint8_t>(addr >> 24),
            static_cast<std::uint8_t>(addr >> 16),
            static_cast<std::uint8_t>(addr >> 8),
            static_cast<std::uint8_t>(addr),
        };
    };

    inline std::uint16_t calculateChecksum(const std::vector<std::uint8_t> &data, int begin, int end) {
        std::uint32_t sum = 0;
        for (int i = begin; i < end && i < data.size(); i+=2) {
            sum += static_cast<std::uint32_t>(data[i]) << 8 | static_cast<std::uint32_t>(data[i+1]);
        }
        return static_cast<std::uint16_t>((sum + (sum >> 16)) & 0x0000FFFF);
    }

    inline bool verifyChecksum(const std::vector<std::uint8_t> &data, int begin, int end) {
        return calculateChecksum(data, begin, end) == 0xFFFF;
    }
}

#endif //SUIKA_IPUTILS_H
