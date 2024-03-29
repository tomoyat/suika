#include "lib/Logger.h"
#include "lib/Util.h"
#include "lib/EtherDevice.h"
#include "lib/Interrupt.h"
#include "lib/IpUtils.h"
#include "lib/Arp.h"
#include "lib/IpNetworkInterface.h"
#include "lib/Ipv4.h"
#include "lib/Icmp.h"
#include "lib/RouteTable.h"
#include "lib/Udp.h"
#include <chrono>

constexpr char tun_device[] = "/dev/net/tun";

constexpr char user[] = "root";
constexpr char tun_device_name[] = "suika_device";
constexpr char tun_ip_range[] = "192.0.2.1/24";
const std::uint32_t tun_ip = suika::ipUtils::Ipv4strToUint32("192.0.2.1");

static volatile sig_atomic_t terminate = 0;

void on_signal(int s) {
    terminate = 1;
    kill(getpid(), SIGUSR2);
}

void setupTunDevice() {
    suika::util::exec(std::format("ip tuntap add mode tap user {} name {}", user, tun_device_name));
    suika::util::exec(std::format("ip addr add {} dev {}", tun_ip_range, tun_device_name));
    suika::util::exec(std::format("ip link set {} up", tun_device_name));
};

void protocolQueueInit() {
    suika::protocol::protocolQueues.insert(
        std::make_pair(suika::protocol::arpType,
                       std::queue<std::shared_ptr<suika::protocol::ProtocolData> >())
    );
    suika::protocol::protocolHandlers[suika::protocol::arpType] = std::make_shared<
        suika::protocol::arp::ArpProtocolHandler>();

    suika::protocol::protocolQueues.insert(
        std::make_pair(
            suika::protocol::ipType,
            std::queue<std::shared_ptr<suika::protocol::ProtocolData> >()
        )
    );
    suika::protocol::protocolHandlers[suika::protocol::ipType] = std::make_shared<
        suika::protocol::ipv4::Ipv4ProtocolHandler>();

    suika::protocol::ipv4::eventHandlers.push_back(
        std::make_shared<suika::protocol::udp::UdpEventHandler>());

};

void ipProtocolInit() {
    suika::protocol::ipv4::protocolHandlers[suika::protocol::ipv4::ICMP_TYPE] =
            std::make_shared<suika::protocol::icmp::IcmpHandler>();
    suika::protocol::ipv4::protocolHandlers[suika::protocol::ipv4::UDP_TYPE] =
            std::make_shared<suika::protocol::udp::UdpHandler>();
}

int main() {

    signal(SIGINT, on_signal);
    signal(SIGTERM, on_signal);

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

    auto sock = suika::protocol::udp::UdpSocket::open();
    if (!sock.has_value()) {
        throw std::runtime_error("socket error");
    }
    auto s = sock.value();
    s.bind("192.0.2.2", 5555);

    while (!terminate) {
         try {
             std::vector<std::uint8_t> buffer(10000, 0);
             std::uint32_t srcIp;
             std::uint16_t srcPort;
             auto len = s.recvFrom(buffer, srcIp, srcPort);

             if (len == -1) {
                 continue;
             }
             buffer.resize(len);

             suika::logger::info(std::format("resize buffer size = {}", buffer.size()));

             std::string str;
             for (int i = 0; i < len; i++) {
                 str += (char)buffer[i];
             }

             suika::logger::info(std::format("UDP INPUT: ip = {}, port = {}, payload = {}", suika::ipUtils::Uint32ToIpv4str(srcIp), srcPort,
                 str
                 ));

             s.sendTo(buffer, suika::ipUtils::Uint32ToIpv4str(srcIp), srcPort);

         } catch (const std::runtime_error &e) {
             suika::logger::error(std::format("fail send to. {}", e.what()));
         }
    }
    suika::logger::info("start finish");

    intr.shutdown();
    return 0;
}
