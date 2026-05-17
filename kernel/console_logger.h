#ifndef CONSOLE_LOGGER_H
#define CONSOLE_LOGGER_H

#include <string>

class ConsoleLogger {
public:
    static void info(const std::string& msg);
    static void warn(const std::string& msg);
    static void error(const std::string& msg);
};

#endif
