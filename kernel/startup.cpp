#include "startup.h"
#include "console_logger.h"

void Startup::showBootLogo() {
    ConsoleLogger::info("STARTUP: Kaizor Boot Logo Displayed");
}

void Startup::runChecks() {
    ConsoleLogger::info("STARTUP: Running system checks (fsck, disk scan)");
}

void Startup::launchDesktop() {
    ConsoleLogger::info("STARTUP: Launching desktop compositor + shell");
}
