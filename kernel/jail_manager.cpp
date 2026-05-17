#include "jail_manager.h"
#include "console_logger.h"

void JailManager::init() {
    ConsoleLogger::info("JAIL: subsystem initialized");
}

bool JailManager::createJail(const std::string& name) {
    ConsoleLogger::info("JAIL: Created -> " + name);
    return true;
}

bool JailManager::startJail(const std::string& name) {
    ConsoleLogger::info("JAIL: Started -> " + name);
    return true;
}
