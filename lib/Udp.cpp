#include "IpUtils.h"
#include "Udp.h"

#include "Logger.h"
#include "RouteTable.h"
#include "UdpData.h"
#include "UdpControlBlock.h"

namespace suika::protocol::udp {
    int suika::protocol::udp::UdpHandler::handle(
        std::shared_ptr<suika::protocol::ipv4::Ipv4ProtocolData> protocolDataPtr) {
        auto payload = UdpData(protocolDataPtr->data);

        auto pseudo = PseudoHeader();
        pseudo.srcIp(protocolDataPtr->src);
        pseudo.destIp(protocolDataPtr->dst);
        pseudo.protocol(suika::protocol::ipv4::UDP_TYPE);
        pseudo.length(payload.data.size());

        const std::uint16_t pseudoChecksum = suika::ipUtils::calculateChecksum(pseudo.data, 0, pseudo.data.size());
        const std::uint16_t payloadChecksum = suika::ipUtils::calculateChecksum(payload.data, 0, payload.data.size());
        std::vector<std::uint16_t> tmpVec = {pseudoChecksum, payloadChecksum};
        const std::uint16_t checksum = ~suika::ipUtils::calculateChecksumUint16(tmpVec, 0, tmpVec.size());

        if (checksum != 0) {
            suika::logger::error(std::format("udp checksum error invalid payload {}", payload.info()));
            throw std::runtime_error(std::format("udp checksum error invalid payload {}", payload.info()));
        }
        suika::logger::info(std::format("udp payload arrived {}", payload.info()));


        pcb::UdpPcbManager *pcbMgr = pcb::UdpPcbManager::getInstance();
        std::unique_lock<std::mutex> lck(pcbMgr->mutex);
        auto udpPcb = pcbMgr->select(protocolDataPtr->dst, payload.dstPort());
        if (!udpPcb.has_value()) {
            suika::logger::error(std::format("no control block for the udp destination {}", payload.info()));
            return -1;
        }
        auto &pcb = udpPcb.value().get();
        std::lock_guard<std::mutex> lk(pcb.mutex);
        lck.unlock();

        auto endpoint = ipv4::IpEndpoint {protocolDataPtr->src, payload.srcPort()};
        auto pcbPayload = pcb::UdpPcbPayload {
            payload.payload(),
            endpoint
        };
        pcb.que.push(pcbPayload);
        pcb.cv.notify_all();
        return 0;
    }

    int UdpEventHandler::handle() {
        auto udpPcbMgr = pcb::UdpPcbManager::getInstance();
        suika::logger::info("handle interrupted on udp");
        std::lock_guard<std::mutex>(udpPcbMgr->mutex);
        udpPcbMgr->interruptAll();
        return 0;
    }


    UdpSocket::UdpSocket(const int id_, const int pcbId_)
        : id(id_), pcbId(pcbId_) {
    }

    std::expected<int, int> UdpSocket::bind(const std::string &ipv4str,
                                            const std::uint16_t port) {
        std::uint32_t ipv4 = suika::ipUtils::Ipv4strToUint32(ipv4str);

        pcb::UdpPcbManager *pcbMgr = pcb::UdpPcbManager::getInstance();
        std::unique_lock<std::mutex> lck(pcbMgr->mutex);
        auto udpPcb = pcbMgr->select(ipv4, port);
        if (udpPcb.has_value()) {
            return std::unexpected{-1};
        }

        auto &pcbRef = pcbMgr->get(pcbId);

        std::lock_guard<std::mutex> lc(pcbRef.mutex);

        pcbRef.endpoint.addr = ipv4;
        pcbRef.endpoint.port = port;
        return 0;
    }

    int UdpSocket::sendTo(const std::vector<std::uint8_t> &payload,
                          std::string ipv4str, std::uint32_t port) {
        auto ipAddr = ipUtils::Ipv4strToUint32(ipv4str);
        ipv4::IpEndpoint target;
        target.addr = ipAddr;
        target.port = port;
        pcb::UdpPcbManager *pcbMgr = pcb::UdpPcbManager::getInstance();
        std::unique_lock<std::mutex> lck(pcbMgr->mutex);

        auto &pcbRef = pcbMgr->get(pcbId);
        if (pcbRef.state != pcb::UdpControlBlockState::OPEN) {
            throw std::runtime_error("pcb not found");
        }
        std::unique_lock<std::mutex> lc(pcbRef.mutex);

        ipv4::IpEndpoint local;
        if (pcbRef.endpoint.addr == ipv4::IP_ADDR_ANY) {
            auto route = suika::routeTable::routeTable.lookup(ipAddr);
            local.addr = route.interface->unicast;
        } else {
            local.addr = pcbRef.endpoint.addr;
        }

        if (pcbRef.endpoint.port == 0) {
            for (std::uint16_t p = SOURCE_PORT_MIN; p <= SOURCE_PORT_MAX; p++) {
                auto tmpPcb = pcbMgr->select(local.addr, p);
                if (tmpPcb.has_value()) {
                    continue;
                }
                pcbRef.endpoint.port = p;
                break;
            }
            if (pcbRef.endpoint.port == 0) {
                throw std::runtime_error("cant assign port dimamically");
            }
        }
        local.port = pcbRef.endpoint.port;
        lck.unlock();
        lc.unlock();
        suika::logger::info(std::format("call udp output src ip = {}, port = {}, dest ip = {}, port = {}",
                                        ipUtils::Uint32ToIpv4str(local.addr), local.port,
                                        ipUtils::Uint32ToIpv4str(target.addr), target.port
        ));
        return udp_output(local, target, payload);
    }

    int UdpSocket::recvFrom(std::vector<std::uint8_t> &buf, std::uint32_t &ipv4,
                            std::uint16_t &port) {
        pcb::UdpPcbManager *udpPcbMgr = pcb::UdpPcbManager::getInstance();
        std::unique_lock<std::mutex> lck(udpPcbMgr->mutex);
        pcb::UdpControlBlock &pcb = udpPcbMgr->get(pcbId);
        lck.unlock();

        std::unique_lock<std::mutex> lc(pcb.mutex);
        pcb.cv.wait(lc, [&] { return !pcb.que.empty() || pcb.interrupted == 1; });
        suika::logger::info("wake up udp recv from");
        if (pcb.interrupted == 1) {
            return -1;
        }
        auto &payload = pcb.que.front();
        std::copy(payload.data.begin(), payload.data.end(), buf.begin());
        pcb.que.pop();

        ipv4 = payload.endpoint.addr;
        port = payload.endpoint.port;

        return static_cast<int>(payload.data.size());
    }

    std::expected<UdpSocket, int> UdpSocket::open() {
        pcb::UdpPcbManager *udpPcbMgr = pcb::UdpPcbManager::getInstance();
        std::lock_guard<std::mutex> lck(udpPcbMgr->mutex);
        auto udpPcb = udpPcbMgr->alloc();

        if (udpPcb.has_value()) {
            return UdpSocket(udpPcb.value().get().id, udpPcb.value().get().id);
        }
        return std::unexpected{-1};
    }

    int udp_output(const suika::protocol::ipv4::IpEndpoint &src,
                   const suika::protocol::ipv4::IpEndpoint &dest,
                   const std::vector<std::uint8_t> &data) {
        auto payload = suika::protocol::udp::UdpData();
        payload.srcPort(src.port);
        payload.dstPort(dest.port);
        payload.payload(data);
        payload.checksum(0);
        payload.length(payload.data.size());

        auto pseudo = PseudoHeader();
        pseudo.srcIp(src.addr);
        pseudo.destIp(dest.addr);
        pseudo.protocol(suika::protocol::ipv4::UDP_TYPE);
        pseudo.length(payload.data.size());

        const std::uint16_t pseudoChecksum = suika::ipUtils::calculateChecksum(pseudo.data, 0, pseudo.data.size());
        const std::uint16_t payloadChecksum = suika::ipUtils::calculateChecksum(payload.data, 0, payload.data.size());
        std::vector<std::uint16_t> tmpVec = {pseudoChecksum, payloadChecksum};
        const std::uint16_t checksum = ~suika::ipUtils::calculateChecksumUint16(tmpVec, 0, tmpVec.size());
        payload.checksum(checksum);

        suika::logger::info(std::format("call ipv4 output. pseudo | {}", pseudo.info()));
        suika::logger::info(std::format("call ipv4 output. payload | {}", payload.info()));

        return suika::protocol::ipv4::ipv4_output(suika::protocol::ipv4::UDP_TYPE, payload.data, src.addr, dest.addr);
    }
}
