#include "kaizor_init.h"
#include "console_logger.h"
#include "hardware.h"
#include "mount_manager.h"
#include "devfs_manager.h"
#include "sysctl_manager.h"
#include "jail_manager.h"
#include "freebsd_bridge.h"
#include "init_system.h"
#include "startup.h"

void KaizorInit::boot() {

    ConsoleLogger::info("========== KAIZOR INIT (PID 1) ==========");

    Hardware hw;
    hw.detectCPU();
    hw.detectRAM();
    hw.detectGPU();
    hw.detectDisks();
    hw.detectInput();

    SysctlManager sysctl;
    sysctl.loadDefaults();
    sysctl.applyPerformanceProfile();
    sysctl.applySecurityProfile();

    MountManager mount;
    mount.mountRoot("/dev/ada0p2");
    mount.mountDevfs();
    mount.mountProcfs();
    mount.mountTmpfs();
    mount.mountHome();

    DevFSManager devfs;
    devfs.init();
    devfs.applyRules();
    devfs.allowGPU();
    devfs.allowInput();

    JailManager jail;
    jail.init();
    jail.createJail("kaizor-sandbox");

    FreeBSDBridge freebsd;
    freebsd.detectKernel();
    freebsd.loadKernelModules();
    freebsd.prepareUserspace();

    InitSystem init;
    init.loadServices();
    init.startCoreServices();
    init.startNetworkServices();
    init.startAudioServices();
    init.startDesktopServices();

    Startup startup;
    startup.showBootLogo();
    startup.runChecks();

    freebsd.startLogin();
    freebsd.startDesktopSession();

    startup.launchDesktop();

    ConsoleLogger::info("========== KAIZOR SYSTEM READY ==========");
}
