#include <string>
#include <fstream>
#include <memory>
#include <cstdio>
#include "Logger.h"

namespace suika::util {
    std::string exec(const std::string &cmd) {
        std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
        if (!pipe) {
            throw std::runtime_error("error execute command");
        }
        char buffer[128];
        std::string result;
        while (!feof(pipe.get())) {
            if (fgets(buffer, 128, pipe.get()) != nullptr)
                result += buffer;
        }
        suika::logger::info(std::format("execute command. {}", cmd));
        suika::logger::info(std::format("execute command result. {}", result));
        return result;
    }
}
