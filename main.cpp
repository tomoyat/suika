#include "Logger.h"
#include "Util.h"
#include "EtherDevice.h"
#include "Interrupt.h"
#include "Arp.h"
#include "IpNetworkInterface.h"
#include "Ipv4.h"
#include "Icmp.h"
#include "RouteTable.h"
#include <chrono>

constexpr char tun_device[] = "/dev/net/tun";

constexpr char user[] = "root";
constexpr char tun_device_name[] = "suika_device";
constexpr char tun_ip_range[] = "192.0.2.1/24";
const std::uint32_t tun_ip = suika::ipUtils::Ipv4strToUint32("192.0.2.1");

void setupTunDevice() {
    suika::util::exec(std::format("ip tuntap add mode tap user {} name {}", user, tun_device_name));
    suika::util::exec(std::format("ip addr add {} dev {}", tun_ip_range, tun_device_name));
    suika::util::exec(std::format("ip link set {} up", tun_device_name));
};

void protocolQueueInit() {
    suika::protocol::protocolQueues.insert(
            std::make_pair(suika::protocol::arpType,
                           std::queue<std::shared_ptr<suika::protocol::ProtocolData>>())
    );
    suika::protocol::protocolHandlers[suika::protocol::arpType] = std::make_shared<suika::protocol::arp::ArpProtocolHandler>();

    suika::protocol::protocolQueues.insert(
            std::make_pair(
                    suika::protocol::ipType,
                    std::queue<std::shared_ptr<suika::protocol::ProtocolData>>()
            )
    );
    suika::protocol::protocolHandlers[suika::protocol::ipType] = std::make_shared<suika::protocol::ipv4::Ipv4ProtocolHandler>();
};

void ipProtocolInit() {
    suika::protocol::ipv4::protocolHandlers[suika::protocol::ipv4::ICMP_TYPE] =
            std::make_shared<suika::protocol::icmp::IcmpHandler>();
}

int main() {
    setupTunDevice();

    protocolQueueInit();
    ipProtocolInit();

    std::shared_ptr<suika::device::ether::EtherDevice> etherDevicePtr =
            std::make_shared<suika::device::ether::EtherDevice>(
                    std::string{tun_device}, std::string{tun_device_name}, "00:00:5e:00:53:01");
    etherDevicePtr->open();

    std::array<std::uint8_t, suika::ether::ETHER_ADDR_LEN> address{};
    etherDevicePtr->fetchMacAddress(address);
    etherDevicePtr->setSelfPtr(etherDevicePtr);

    auto ipInterfacePtr = std::make_shared<suika::network::IpNetworkInterface>(
            suika::network::INTERFACE_FAMILY_IP,
            "192.0.2.2",
            "255.255.255.0"
    );
    ipInterfacePtr->registerDevice(etherDevicePtr);
    etherDevicePtr->addNetworkInterface(ipInterfacePtr);

    suika::routeTable::routeTable.add(
            suika::routeTable::Route{
                    suika::protocol::ipv4::IP_ADDR_ANY,
                    suika::protocol::ipv4::IP_ADDR_ANY,
                    tun_ip,
                    ipInterfacePtr
            }
    );

    suika::logger::info(
            std::format("ether device address : {}", suika::device::ether::addressToString(etherDevicePtr->address)));

    suika::logger::info(std::format("ip interface : {}", ipInterfacePtr->info()));
    auto intr = suika::interrupt::Interrupt();

    intr.init();
    intr.addDevice(etherDevicePtr);

    intr.run();
    suika::logger::info("wait 60 sec ");

    for (int i = 0; i < 10; i++) {
        suika::protocol::icmp::sendIcmp(1, i, tun_ip, ipInterfacePtr);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(60000));

    intr.shutdown();
    return 0;
}
