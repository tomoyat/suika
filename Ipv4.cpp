#include "Ipv4.h"
#include "Ipv4Data.h"
#include "Logger.h"
#include "IpUtils.h"
#include "IpNetworkInterface.h"

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

        // TODO
        // version check
        // header length check
        // total length check
        // checksum check
        if (!ipv4Packet.verifyHeader()) {
            suika::logger::info("invalid header.");
            throw std::runtime_error("invalid header");
        }
        // fragment check
        auto protocol = ipv4Packet.protocol();

        std::uint32_t headerLength = ipv4Packet.headerLength() * 4;
        auto total = ipv4Packet.totalLength();
        auto dataLength = total - headerLength;


        auto data = std::vector<std::uint8_t>(dataLength);
        std::copy(ipv4Packet.data.begin() + headerLength,
                  ipv4Packet.data.begin() + total,
                  data.begin());

        auto interface = protocolDataPtr->devicePtr->getTargetInterface(suika::network::INTERFACE_FAMILY_IP);
        auto ipNetworkInterface = dynamic_pointer_cast<suika::network::IpNetworkInterface>(interface);
        if (ipNetworkInterface) {
            if (ipNetworkInterface->unicast != ipv4Packet.dst()) {
                throw std::runtime_error("ip not match error");
            }
        } else {
            throw std::runtime_error("cast error");
        }

        if (protocolHandlers.find(protocol) == protocolHandlers.end()) {
            suika::logger::error(std::format("ip protocol not found. protocol = {}", protocol));
            return 0;
        }

        auto d = std::make_shared<Ipv4ProtocolData>(
                Ipv4ProtocolData{protocol, data, ipv4Packet.src(), ipv4Packet.dst(), ipNetworkInterface}
        );
        protocolHandlers[protocol]->handle(d);
        // TODO ICMPと仮定してちょっとやってみる
        //auto payload = BinaryPayload{data};
        //auto type = payload.readUint8(0);
        //auto code = payload.readUint8(1);
        //auto checksum = payload.readUint16(2);
        //suika::logger::info(
        //        std::format(
        //                "icmp type = {}, code = {}, checksum = {}", type, code, checksum
        //        )
        //);

        return 0;
    }
}
