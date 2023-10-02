#include "Icmp.h"
#include "IcmpData.h"
#include "Logger.h"
#include "IpUtils.h"

namespace suika::protocol::icmp {
    int suika::protocol::icmp::IcmpHandler::handle(
            std::shared_ptr<suika::protocol::ipv4::Ipv4ProtocolData> protocolDataPtr) {

        auto d = IcmpDataEcho{protocolDataPtr->data};

        suika::logger::info(
                std::format("icmp input type = {}, code = {}, checksum = {}, identifier = {}, sequenceNumber = {}",
                            d.type(), d.code(), d.checksum(), d.identifier(), d.sequenceNumber()));

        // verify

        suika::logger::info(std::format("interface = {}",
                                        suika::ipUtils::Uint32ToIpv4str(protocolDataPtr->ipNetworkInterfacePtr->unicast)));

        return 0;

    }
}
