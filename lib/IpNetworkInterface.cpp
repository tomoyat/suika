#include "IpNetworkInterface.h"

int suika::network::IpNetworkInterface::registerDevice(std::shared_ptr<suika::device::Device> ptr) {
    devicePtr = ptr;
    return 0;
}
