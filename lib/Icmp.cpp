#include "Icmp.h"
#include "IcmpData.h"
#include "Logger.h"
#include "IpUtils.h"
#include "IpNetworkInterface.h"
#include "Error.h"

namespace suika::protocol::icmp {

    int sendIcmp(
            const std::uint16_t identifier,
            const std::uint16_t sequenceNumber,
            const std::uint32_t dst,
            const std::shared_ptr<suika::network::IpNetworkInterface> &ipNetworkInterfacePtr) {
        auto echo = IcmpDataEcho{};
        echo.identifier(identifier);
        echo.sequenceNumber(sequenceNumber);
        echo.type(suika::protocol::icmp::TYPE_ECHO);
        echo.code(0);

        echo.checksum(0);
        auto checksum = ~suika::ipUtils::calculateChecksum(echo.data, 0, echo.data.size());
        echo.checksum(checksum);

        suika::logger::info(std::format("send icmp {}", echo.info()));
        try {
            suika::protocol::ipv4::ipv4_output(suika::protocol::ipv4::ICMP_TYPE, echo.data,
                                               ipNetworkInterfacePtr->unicast, dst);
        } catch (suika::error::ArpResolveException &e) {
            suika::logger::warn("fail arp");
        }
        return 0;
    }

    int replyIcmp(const IcmpDataEcho &request,
                  const std::uint32_t dst,
                  const std::shared_ptr<suika::network::IpNetworkInterface> &ipNetworkInterfacePtr) {
        auto reply = IcmpDataEcho{request.data};
        reply.type(suika::protocol::icmp::TYPE_ECHO_REPLY);
        reply.checksum(0);

        auto checksum = ~suika::ipUtils::calculateChecksum(reply.data, 0, reply.data.size());
        reply.checksum(checksum);

        suika::logger::info(std::format("icmp reply {}", reply.info()));

        suika::protocol::ipv4::ipv4_output(suika::protocol::ipv4::ICMP_TYPE, reply.data, ipNetworkInterfacePtr->unicast, dst);

        return 0;
    }

    int suika::protocol::icmp::IcmpHandler::handle(
            std::shared_ptr<suika::protocol::ipv4::Ipv4ProtocolData> protocolDataPtr) {

        auto d = IcmpDataEcho{protocolDataPtr->data};

        suika::logger::info(
                std::format(
                        "icmp input type = {}, code = {}, checksum = {}, identifier = {}, sequenceNumber = {}, src = {}",
                        d.type(), d.code(), d.checksum(), d.identifier(), d.sequenceNumber(),
                        suika::ipUtils::Uint32ToIpv4str(protocolDataPtr->src)));

        if (!suika::ipUtils::verifyChecksum(d.data, 0, d.data.size())) {
            suika::logger::error("invalid icmp payload");
            throw std::runtime_error("invalid icmp payload");
        }

        suika::logger::info(std::format("interface = {}",
                                        suika::ipUtils::Uint32ToIpv4str(
                                                protocolDataPtr->ipNetworkInterfacePtr->unicast)));

        switch (d.type()) {
            case suika::protocol::icmp::TYPE_ECHO:
                replyIcmp(d, protocolDataPtr->src, protocolDataPtr->ipNetworkInterfacePtr);
                break;
            case suika::protocol::icmp::TYPE_ECHO_REPLY:
                suika::logger::info(std::format("receive icmp reply. {}", d.info()));
                break;
            default:
                break;
        }
        return 0;
    }
}
