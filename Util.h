#ifndef SUIKA_UTIL_H
#define SUIKA_UTIL_H

#include <string>
#include <cstdarg>
#include <string>
#include <fstream>
#include <memory>
#include <cstdio>
#include <csignal>
#include "Logger.h"

namespace suika::util {

    static const int IRQ_BASE = SIGRTMIN + 1;
    static const int ETHER_TAN_IRQ = IRQ_BASE + 1;


    std::string exec(const std::string &cmd);
}

#endif //SUIKA_UTIL_H
