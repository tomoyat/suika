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

    void log(level level,
             std::string_view message,
             std::source_location location);

    void error(std::string_view message, std::source_location location = std::source_location::current());

    void warn(std::string_view message, std::source_location location = std::source_location::current());

    void info(std::string_view message, std::source_location location = std::source_location::current());

    void debug(std::string_view message, std::source_location location = std::source_location::current());
}
#endif //SUIKA_LOGGER_H
