#include "Icmp.h"
#include "Logger.h"

namespace suika::protocol::icmp {
    int suika::protocol::icmp::IcmpHandler::handle(
            std::shared_ptr<suika::protocol::ipv4::Ipv4ProtocolData> protocolDataPtr) {

        suika::logger::info("call icmp");
        return 0;
    }
}
