#include "Logger.h"
#include "Util.h"
#include "EtherDevice.h"
#include "Interrupt.h"
#include "Arp.h"
#include "IpNetworkInterface.h"
#include <chrono>

constexpr char tun_device[] = "/dev/net/tun";

constexpr char user[] = "root";
constexpr char tun_device_name[] = "suika_device";
constexpr char tun_ip_range[] = "192.0.2.1/24";

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
    suika::logger::info(std::format("main protocol queues address = {}",
                                    static_cast<void *>(&suika::protocol::protocolQueues[suika::protocol::arpType])));

    suika::protocol::protocolHandlers[suika::protocol::arpType] = std::make_shared<suika::protocol::arp::ArpProtocolHandler>();
};

int main() {
    setupTunDevice();

    protocolQueueInit();

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

    suika::logger::info(
            std::format("ether device address : {}", suika::device::ether::addressToString(etherDevicePtr->address)));

    suika::logger::info(std::format("ip interface : {}", ipInterfacePtr->info()));
    auto intr = suika::interrupt::Interrupt();

    intr.init();
    intr.addDevice(etherDevicePtr);

    intr.run();
    suika::logger::info("wait 60 sec ");

    std::this_thread::sleep_for(std::chrono::milliseconds(60000));

    intr.shutdown();
    return 0;
}
