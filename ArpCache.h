#ifndef SUIKA_ARPCACHE_H
#define SUIKA_ARPCACHE_H

#include <vector>
#include <cstdint>
#include <optional>
#include <algorithm>
#include <memory>

namespace suika::protocol::arp {
    struct CacheData {
        std::uint32_t protocolAddress;
        std::vector<uint8_t> hardwareAddress;
    };
    struct ArpCache{
        std::vector<suika::protocol::arp::CacheData> cache;
        std::mutex arpCacheMutex;

        std::optional<CacheData> select(std::uint32_t protocolAddress) {
            std::lock_guard<std::mutex> lock(arpCacheMutex);
            auto op = [protocolAddress](const CacheData& c) { return c.protocolAddress == protocolAddress; };
            auto it = std::find_if(cache.begin(), cache.end(), op);
            if (it == cache.end()) {
                return std::nullopt;
            }
            return *it;
        }

        void create(const CacheData& c) {
            if (auto d = select(c.protocolAddress)) {
                return;
            }
            std::lock_guard<std::mutex> lock(arpCacheMutex);
            cache.push_back(c);
        }
    };

    inline ArpCache arpCache;
}

#endif //SUIKA_ARPCACHE_H
