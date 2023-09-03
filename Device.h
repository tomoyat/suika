#ifndef SUIKA_DEVICE_H
#define SUIKA_DEVICE_H

#include <thread>
#include <memory>
#include <vector>

#include "NetworkInterface.h"

namespace suika::device {
    struct Device {
        std::vector<std::shared_ptr<suika::network::NetworkInterface>> networkInterfaces;

        virtual int open() = 0;

        virtual int handler(pthread_t) = 0;

        virtual int getIrq() = 0;

        virtual std::string getInfo() = 0;

        virtual int addNetworkInterface(std::shared_ptr<suika::network::NetworkInterface> interfacePtr) = 0;

        virtual void setSelfPtr(std::shared_ptr<suika::device::Device> selfPtr_) = 0;

        virtual std::shared_ptr<suika::device::Device> getSelfPtr() = 0;

        virtual ~Device() = default;

        std::shared_ptr<suika::network::NetworkInterface> getTargetInterface(int family) {
            for (auto &interface: networkInterfaces) {
                if (interface->family == family) {
                    return interface;
                }
            }
            throw std::runtime_error("interface not found");
        }
    };
};

#endif //SUIKA_DEVICE_H