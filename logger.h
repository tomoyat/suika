#ifndef SUIKA_LOGGER_H
#define SUIKA_LOGGER_H

#include <iostream>
#include <source_location>
#include <string>
#include <chrono>
#include <format>

namespace suika::logger {

    constexpr std::string_view timezone{"Asia/Tokyo"};

    enum class level : char {
        Debug = 'D',
        Info = 'I',
        Warning = 'W',
        Error = 'E',
    };

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

    void error(const std::string_view message) {
        log(level::Error, message);
    }

    void warn(const std::string_view message) {
        log(level::Warning, message);
    }

    void info(const std::string_view message) {
        log(level::Info, message);
    }

    void debug(const std::string_view message) {
        log(level::Debug, message);
    }
}
#endif //SUIKA_LOGGER_H
