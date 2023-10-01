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
}

#endif //SUIKA_IPUTILS_H
