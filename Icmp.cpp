#include "Icmp.h"
#include "IcmpData.h"
#include "Logger.h"
#include "IpUtils.h"
#include "IpNetworkInterface.h"

namespace suika::protocol::icmp {

    int replyIcmp(const IcmpDataEcho &request,
                  const std::shared_ptr<suika::network::IpNetworkInterface> ipNetworkInterfacePtr) {
        auto reply = IcmpDataEcho{request.data};
        reply.type(0);
        reply.checksum(0);

        auto checksum = ~suika::ipUtils::calculateChecksum(reply.data, 0, reply.data.size());
        reply.checksum(checksum);

        suika::logger::info(std::format("icmp reply {}", reply.info()));



        return 0;
    }

    int suika::protocol::icmp::IcmpHandler::handle(
            std::shared_ptr<suika::protocol::ipv4::Ipv4ProtocolData> protocolDataPtr) {

        auto d = IcmpDataEcho{protocolDataPtr->data};

        suika::logger::info(
                std::format("icmp input type = {}, code = {}, checksum = {}, identifier = {}, sequenceNumber = {}",
                            d.type(), d.code(), d.checksum(), d.identifier(), d.sequenceNumber()));

        if (!suika::ipUtils::verifyChecksum(d.data, 0, d.data.size())) {
            suika::logger::error("invalid icmp payload");
            throw std::runtime_error("invalid icmp payload");
        }

        suika::logger::info(std::format("interface = {}",
                                        suika::ipUtils::Uint32ToIpv4str(protocolDataPtr->ipNetworkInterfacePtr->unicast)));

        replyIcmp(d, protocolDataPtr->ipNetworkInterfacePtr);
        return 0;

    }
}
