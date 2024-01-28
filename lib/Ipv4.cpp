#include "Ipv4.h"
#include "Ipv4Data.h"
#include "Logger.h"
#include "IpUtils.h"
#include "IpNetworkInterface.h"
#include "RouteTable.h"
#include "ArpCache.h"
#include "Protocol.h"
#include "Error.h"
#include "Arp.h"

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
        suika::logger::info(std::format("ip input packet = {}", ipv4Packet.info()));
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

        std::vector<std::uint8_t> data = ipv4Packet.payload();

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

        return 0;
    }

    int ipv4_output(std::uint8_t protocol, const std::vector<std::uint8_t> &data, std::uint32_t src, std::uint32_t dst) {
        if (src == IP_ADDR_ANY && dst == IP_ADDR_BROADCAST) {
            suika::logger::error("src is required if broadcast request");
            throw std::runtime_error("src is required");
        }

        auto route = suika::routeTable::routeTable.lookup(dst);

        if (src != IP_ADDR_ANY && src != route.interface->unicast) {
            suika::logger::error(
                    std::format("ip not match src = {}, interface unicast = {}",
                                suika::ipUtils::Uint32ToIpv4str(src),
                                suika::ipUtils::Uint32ToIpv4str(route.interface->unicast))
            );
            throw std::runtime_error("ip not match");
        }
        std::uint32_t gateway = route.gateway;

        // ipのデータを作る
        auto ipv4Packet = Ipv4Data();
        ipv4Packet.typeOfService(0);

        auto total = ipv4Packet.headerLengthByte() + data.size();
        ipv4Packet.totalLength(total);

        auto id = ip_gen_id();
        ipv4Packet.identification(id);

        ipv4Packet.flagsAndFragmentOffset(0);
        ipv4Packet.timeToLive(0xff);
        ipv4Packet.protocol(protocol);
        ipv4Packet.checksum(0);
        suika::logger::info(std::format("ipv4 output: src = {}, dst = {}",
                                        suika::ipUtils::Uint32ToIpv4str(src),
                                        suika::ipUtils::Uint32ToIpv4str(dst)));

        ipv4Packet.src(src);
        ipv4Packet.dst(dst);

        ipv4Packet.setCheckSum();
        suika::logger::info(std::format("output ip packet = {}", ipv4Packet.info()));

        ipv4Packet.payload(data);


        // ARP cacheの探索をして、interfaceのetherを叩く
        try {
            auto hardwareAddr = suika::protocol::arp::arpResolve(route.interface, gateway);
            route.interface->devicePtr->transmit(ipv4Packet.data, hardwareAddr, suika::protocol::ipType);
        } catch (suika::error::ArpResolveException &e) {
            throw e;
        }
        return 0;
    }

    std::uint16_t ip_gen_id(void) {
        static std::mutex ipGenIdMutex;
        static uint16_t id = 128;
        uint16_t ret;
        std::lock_guard<std::mutex> lock(ipGenIdMutex);

        ret = id++;
        return ret;
    };
}
