#include "logger.h"
#include "util.h"
#include "EtherDevice.h"

constexpr char tun_device[] = "/dev/net/tun";

constexpr char user[] = "root";
constexpr char tun_device_name[] = "suika_device";
constexpr char tun_ip_range[] = "192.0.2.1/24";

void setupTunDevice() {
    suika::util::exec(std::format("ip tuntap add mode tap user {} name {}", user, tun_device_name));
    suika::util::exec(std::format("ip addr add {} dev {}", tun_ip_range, tun_device_name));
    suika::util::exec(std::format("ip link set {} up", tun_device_name));
};

int main() {
    setupTunDevice();

    auto etherDevice = suika::device::ether::EtherDevice(
            std::string{tun_device}, std::string{tun_device_name}, "00:00:5e:00:53:01");
    suika::logger::info(std::format("ether device address : {}", suika::device::ether::addressToString(etherDevice.address)));
    etherDevice.open();

    return 0;
}
