#include "logger.h"

#include <iostream>
#include <source_location>
#include <string>
#include <chrono>
#include <format>

namespace suika::logger {

    void log(level const level,
             const std::string_view message,
             const std::source_location location = std::source_location::current()) {

        std::chrono::zoned_time zonedTime{timezone, std::chrono::system_clock::now()};
        auto formatTime = std::format("{:%F %T %Z}", zonedTime);
        auto functionInfo = std::format("{}({}) `{}`",
                                        location.file_name(), location.line(), location.function_name());
        std::clog
                << std::format("[{}] {} | {} | {}",
                               static_cast<char>(level), formatTime, functionInfo, message)
                << std::endl;
    }

    void error(const std::string_view message, const std::source_location location) {
        log(level::Error, message, location);
    }

    void warn(const std::string_view message, const std::source_location location) {
        log(level::Warning, message, location);
    }

    void info(const std::string_view message, const std::source_location location) {
        log(level::Info, message, location);
    }

    void debug(const std::string_view message, const std::source_location location) {
        log(level::Debug, message, location);
    }
}