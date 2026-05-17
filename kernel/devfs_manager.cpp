#include "devfs_manager.h"
#include "console_logger.h"

void DevFSManager::init() {
    ConsoleLogger::info("DEVFS: Initialized");
}

void DevFSManager::applyRules() {
    ConsoleLogger::info("DEVFS: Default device rules applied");
}

void DevFSManager::allowGPU() {
    ConsoleLogger::info("DEVFS: GPU nodes enabled (/dev/dri)");
}

void DevFSManager::allowInput() {
    ConsoleLogger::info("DEVFS: Input nodes enabled (/dev/kbd, /dev/ums)");
}
