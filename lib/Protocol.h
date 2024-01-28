#ifndef SUIKA_PROTOCOL_H
#define SUIKA_PROTOCOL_H

#include <queue>
#include <map>
#include <memory>
#include <cstdint>

#include "EtherDevice.h"

namespace suika::protocol {
    // https://www.iana.org/assignments/ieee-802-numbers/ieee-802-numbers.xhtml
    static std::uint16_t arpType = 0x0806;
    static std::uint16_t ipType = 0x800;

    struct ProtocolData {
        std::uint16_t type;
        std::vector<std::uint8_t> data;
        std::shared_ptr<suika::device::Device> devicePtr;
    };
    inline std::map<std::uint16_t, std::queue<std::shared_ptr<ProtocolData> > > protocolQueues{};
    static std::mutex protocolQueuesMutex;

    struct ProtocolHandler {
        virtual int handle(std::shared_ptr<ProtocolData> protocolDataPtr) = 0;

        virtual ~ProtocolHandler() = default;
    };

    static std::map<std::uint16_t, std::shared_ptr<ProtocolHandler> > protocolHandlers;
}

#endif //SUIKA_PROTOCOL_H
