#ifndef SUIKA_ERROR_H
#define SUIKA_ERROR_H

#include <stdexcept>
#include <string>

namespace suika::error {
    class ArpResolveException : public std::runtime_error { ;
    public:
        explicit ArpResolveException(const std::string &_arg) : std::runtime_error(_arg) {}
    };
}

#endif //SUIKA_ERROR_H
