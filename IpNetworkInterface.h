#ifndef SUIKA_IPNETWORKINTERFACE_H
#define SUIKA_IPNETWORKINTERFACE_H

#include <vector>

#include "NetworkInterface.h"

namespace suika::network {
    struct IpNetworkInterface : NetworkInterface {
        explicit IpNetworkInterface(int family_) {
            family = family_;
        }
        std::shared_ptr<suika::device::Device> devicePtr{};

        int registerDevice(std::shared_ptr<suika::device::Device> ptr) override;
    };
}
#endif //SUIKA_IPNETWORKINTERFACE_H
