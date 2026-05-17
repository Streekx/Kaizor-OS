#include "mount_manager.h"
#include "console_logger.h"

bool MountManager::mountRoot(const std::string& device) {
    ConsoleLogger::info("MOUNT: Root filesystem mounting from " + device);
    return true;
}

bool MountManager::mountProcfs() {
    ConsoleLogger::info("MOUNT: procfs mounted at /proc");
    return true;
}

bool MountManager::mountTmpfs() {
    ConsoleLogger::info("MOUNT: tmpfs mounted at /tmp");
    return true;
}

bool MountManager::mountDevfs() {
    ConsoleLogger::info("MOUNT: devfs mounted at /dev");
    return true;
}

bool MountManager::mountHome() {
    ConsoleLogger::info("MOUNT: home filesystem mounted at /home");
    return true;
}
