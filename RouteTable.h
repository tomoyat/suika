#ifndef SUIKA_ROUTETABLE_H
#define SUIKA_ROUTETABLE_H

#include <cstdint>
#include <vector>
#include <memory>

#include "IpNetworkInterface.h"


namespace suika::routeTable {
    struct Route {
        std::uint32_t network;
        std::uint32_t mask;
        std::uint32_t gateway;
        std::shared_ptr<suika::network::IpNetworkInterface> interface;
    };

    struct RouteTable {
        std::vector<Route> routes;

        void add(Route r) {
            routes.push_back(r);
        }

        Route lookup(std::uint32_t dst) {
            return routes[0];
        }
    };

    inline RouteTable routeTable{};
}

#endif //SUIKA_ROUTETABLE_H
