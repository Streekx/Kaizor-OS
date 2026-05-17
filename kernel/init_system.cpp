#include "init_system.h"
#include "console_logger.h"
#include "rc_service.h"

void InitSystem::loadServices() {
    ConsoleLogger::info("INIT: Loading Kaizor init service list");
}

void InitSystem::startCoreServices() {
    RCService rc;
    ConsoleLogger::info("INIT: Starting core services");
    rc.start("devd");
    rc.start("syslogd");
    rc.start("cron");
}

void InitSystem::startNetworkServices() {
    RCService rc;
    ConsoleLogger::info("INIT: Starting network services");
    rc.start("netif");
    rc.start("routing");
    rc.start("dhclient");
}

void InitSystem::startAudioServices() {
    RCService rc;
    ConsoleLogger::info("INIT: Starting audio services");
    rc.start("sndiod");
}

void InitSystem::startDesktopServices() {
    RCService rc;
    ConsoleLogger::info("INIT: Starting desktop services");
    rc.start("dbus");
    rc.start("hald");
}
