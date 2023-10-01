#ifndef SUIKA_ICMP_H
#define SUIKA_ICMP_H

#include "Ipv4.h"

namespace suika::protocol::icmp {
    struct IcmpHandler : suika::protocol::ipv4::ProtocolHandler {
        int handle(std::shared_ptr<suika::protocol::ipv4::Ipv4ProtocolData> protocolDataPtr) override;
    };
}


#endif //SUIKA_ICMP_H
