#include "Ipv4.h"
#include "Ipv4Data.h"
#include "Logger.h"
#include "IpUtils.h"

namespace suika::protocol::ipv4 {
    int Ipv4ProtocolHandler::handle(
            std::shared_ptr<suika::protocol::ProtocolData>
            protocolDataPtr) {

        suika::logger::info(std::format("handle ipv4"));

        auto ipv4Packet = Ipv4Data(protocolDataPtr);

        suika::logger::info(
                std::format(
                        "src ip = {}, dst ip = {}, protocol = {}, headerLength = {}",
                        suika::ipUtils::Uint32ToIpv4str(ipv4Packet.src()),
                        suika::ipUtils::Uint32ToIpv4str(ipv4Packet.dst()),
                        ipv4Packet.protocol(),
                        ipv4Packet.headerLength()
                )
        );

        return 0;
    }
}
