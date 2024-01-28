#ifndef SUIKA_NETWORKINTERFACE_H
#define SUIKA_NETWORKINTERFACE_H

#include <memory>
#include <vector>

namespace suika::device {
    class Device;
}

namespace suika::network {

    inline constexpr int INTERFACE_FAMILY_IP = 1;

    struct NetworkInterface {
        int family{};

        virtual int registerDevice(std::shared_ptr<suika::device::Device> devicePtr) = 0;
    };
}

#endif //SUIKA_NETWORKINTERFACE_H
