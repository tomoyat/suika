#ifndef SUIKA_IPV4_H
#define SUIKA_IPV4_H

#include "Protocol.h"

namespace suika::protocol::ipv4 {

    struct Ipv4ProtocolHandler : suika::protocol::ProtocolHandler {
        int handle(std::shared_ptr<suika::protocol::ProtocolData> protocolDataPtr) override;
    };
}

#endif //SUIKA_IPV4_H
