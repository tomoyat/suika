#include "Arp.h"
#include "Logger.h"
#include "EtherDevice.h"
#include "IpNetworkInterface.h"

namespace suika::protocol::arp {
    std::string macAddressToString(const std::vector<uint8_t> &addr) {
        if (addr.size() != 6) {
            throw std::runtime_error("invalid mac address length");
        }
        return std::format("{:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}", addr[0], addr[1], addr[2], addr[3], addr[4],
                           addr[5]);
    }

    std::string ipV4ToString(const std::vector<uint8_t> &addr) {
        if (addr.size() != 4) {
            throw std::runtime_error(std::format("invalid ipv4 address length. length = {}", addr.size()));
        }
        return std::format("{}.{}.{}.{}", addr[0], addr[1], addr[2], addr[3]);
    }

    int reply(std::shared_ptr<suika::network::IpNetworkInterface> interface, const ArpData& requestData) {
        auto data = arpDataFactory(
                suika::protocol::arp::ARP_HARDWARE_TYPE_ETHER,
                suika::protocol::arp::ARP_PROTOCOL_TYPE_IP,
                suika::protocol::arp::ETHER_ADDRESS_LEN,
                suika::protocol::arp::IP_ADDRESS_LEN,
                suika::protocol::arp::OP_REPLY
        );
        data.setSenderHardwareAddress(interface->devicePtr->getAddress());
        data.setSenderProtocolAddress(interface->getUnicastVector());
        // TODO
        // data.setTargetHardwareAddress(interface->)
    }

    int ArpProtocolHandler::handle(std::shared_ptr<suika::protocol::ProtocolData> protocolData) {
        ArpData arpData{protocolData};

        suika::logger::debug(
                std::format("arp : hardware type={}, protocol type={:04x}", arpData.hardwareType(),
                            arpData.protocolType()));
        suika::logger::debug(
                std::format("arp : hardware length={}, protocol length={}", arpData.hardwareAddressLength(),
                            arpData.protocolAddressLength()));
        suika::logger::debug(std::format("arp : operation code={}", arpData.operationCode()));

        suika::logger::debug(std::format("sender mac address={}, ip={}",
                                         suika::protocol::arp::macAddressToString(arpData.senderHardwareAddress()),
                                         suika::protocol::arp::ipV4ToString(arpData.senderProtocolAddress())));

        suika::logger::debug(std::format("target mac address={}, ip={}",
                                         suika::protocol::arp::macAddressToString(arpData.targetHardwareAddress()),
                                         suika::protocol::arp::ipV4ToString(arpData.targetProtocolAddress())));

        if (arpData.protocolAddressLength() != suika::ether::IP_ADDR_LEN ||
            arpData.protocolType() != suika::ether::ETHER_TYPE_IP) {
            suika::logger::error(std::format("unsupported protocol type = {}", arpData.protocolType()));
            throw std::runtime_error("not support protocol");
        }

        if (arpData.protocolAddressLength() != suika::ether::IP_ADDR_LEN ||
            arpData.protocolType() != suika::ether::ETHER_TYPE_IP) {
            suika::logger::error(std::format("unsupported protocol type = {}", arpData.protocolType()));
            throw std::runtime_error("not support protocol");
        }

        auto targetInterface = protocolData->devicePtr->getTargetInterface(suika::network::INTERFACE_FAMILY_IP);

        if (auto ipInterface = dynamic_pointer_cast<suika::network::IpNetworkInterface>(targetInterface)) {
            auto address = arpData.targetProtocolAddress();
            if (address.size() != suika::ether::IP_ADDR_LEN) {
                throw std::runtime_error(std::format("not support protocol address size = {}", address.size()));
            }
            uint32_t targetProtocolAddress =
                    static_cast<uint32_t>(address[0]) << 24 |
                    static_cast<uint32_t>(address[1]) << 16 |
                    static_cast<uint32_t>(address[2]) << 8 |
                    static_cast<uint32_t>(address[3]);
            suika::logger::info(std::format("unicast = {}, target = {}", ipInterface->unicast, targetProtocolAddress));
            if (targetProtocolAddress != ipInterface->unicast) {
                throw std::runtime_error(std::format("not support protocol address size = {}", address.size()));
            }

            if (arpData.operationCode() == suika::protocol::arp::OP_REQUEST) {
                suika::protocol::arp::reply(ipInterface, arpData);
            }

            /***
             * TODO
             * 自身のinterfaceのアドレスとarpのtargetのアドレスを比較
             * arp cacheを更新
             * arp requestだったらreplyする
             * 返信用のデータを作って、
             * interfaceに紐づけられたdeviceのtransmitをよぶ
             *
             */

        } else {
            throw std::runtime_error("cast error");
        }
        return 0;
    }
}
