#ifndef SUIKA_UDPCONTROLBLOCK_H
#define SUIKA_UDPCONTROLBLOCK_H

#include "vector"
#include "cstdint"
#include "queue"
#include <condition_variable>
#include <mutex>
#include <queue>
#include <vector>
#include <cstdint>
#include <expected>
#include <functional>

#include "Ipv4.h"

namespace suika::protocol::udp::pcb {
    enum class UdpControlBlockState {
        FREE,
        OPEN,
        CLOSING,
    };

    struct UdpPcbPayload {
        std::vector<std::uint8_t> data;
        ipv4::IpEndpoint endpoint;
    };

    class UdpControlBlock {
    public:
        int id;
        UdpControlBlockState state;
        std::queue< UdpPcbPayload > que;
        ipv4::IpEndpoint endpoint;
        std::mutex mutex;
        std::condition_variable cv;

        UdpControlBlock() : id(-1), state(UdpControlBlockState::FREE) {
        }

        UdpControlBlock(const UdpControlBlock &) = delete;

        UdpControlBlock &operator=(const UdpControlBlock &) = delete;
    };

    class UdpPcbManager {
        std::vector<UdpControlBlock> udpPcbList;

        UdpPcbManager();

        ~UdpPcbManager();

    public:
        std::mutex mutex;

        UdpPcbManager(const UdpPcbManager &);

        UdpPcbManager &operator=(const UdpPcbManager &) = delete;

        UdpControlBlock &get(int id_);

        std::expected<std::reference_wrapper<UdpControlBlock>, int> alloc();

        std::expected<std::reference_wrapper<UdpControlBlock>, int> select(
            std::uint32_t ipv4, std::uint16_t port);

        static UdpPcbManager *getInstance();
    };
}

#endif //SUIKA_UDPCONTROLBLOCK_H
