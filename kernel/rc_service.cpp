#include "rc_service.h"
#include "console_logger.h"

bool RCService::start(const std::string& name) {
    ConsoleLogger::info("RC: Starting service -> " + name);
    return true;
}

bool RCService::stop(const std::string& name) {
    ConsoleLogger::info("RC: Stopping service -> " + name);
    return true;
}

bool RCService::enable(const std::string& name) {
    ConsoleLogger::info("RC: Enabling service -> " + name);
    return true;
}
