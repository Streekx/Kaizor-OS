#include <iostream>

#include "boot/boot.h"
#include "boot/splash.h"
#include "boot/login_manager.h"

#include "graphics/renderer.h"
#include "graphics/display_server.h"
#include "graphics/compositor.h"
#include "graphics/animations.h"

#include "graphics/sdl_backend.h"
#include "graphics/surface_manager.h"
#include "graphics/gpu_renderer.h"

#include "desktop/desktop_shell.h"
#include "desktop/launcher.h"
#include "desktop/notifications.h"
#include "desktop/notification_center.h"
#include "desktop/wallpaper_engine.h"
#include "desktop/dock_runtime.h"

#include "window_manager/window_manager.h"
#include "window_manager/window.h"
#include "window_manager/taskbar.h"
#include "window_manager/dock.h"
#include "window_manager/workspace.h"

#include "input/input_manager.h"
#include "input/keyboard.h"
#include "input/mouse.h"

#include "apps/file_manager/file_manager.h"
#include "apps/settings/settings.h"
#include "apps/terminal/terminal.h"

#include "core/process_manager.h"
#include "core/session_manager.h"
#include "core/filesystem.h"
#include "core/package_manager.h"
#include "core/service_manager.h"

#include "themes/theme_engine.h"

#include "gui/start_menu.h"
#include "gui/desktop_icon.h"
#include "gui/app_launcher.h"
#include "gui/button.h"
#include "gui/titlebar.h"
#include "gui/font_renderer.h"
#include "gui/cursor.h"
#include "gui/wallpaper.h"
#include "gui/widget_panel.h"

#include "system/power_manager.h"
#include "system/lock_screen.h"
#include "system/system_monitor.h"

#include "networking/network_manager.h"
#include "networking/wifi.h"
#include "networking/ethernet.h"
#include "networking/dns.h"

#include "audio/audio_server.h"
#include "audio/mixer.h"
#include "audio/sound_driver.h"
#include "audio/media.h"

#include "kernel/freebsd_bridge.h"
#include "kernel/init_system.h"
#include "kernel/startup.h"
#include "kernel/hardware.h"

using namespace std;

int main() {

    cout << "========== KAIZOR OS =========="
         << endl;

    // HARDWARE
    Hardware hardware;

    hardware.detectCPU();

    hardware.detectGPU();

    hardware.detectRAM();

    // FREEBSD
    FreeBSDBridge freebsd;

    freebsd.detectKernel();

    freebsd.mountSystem();

    freebsd.startUserspace();

    // INIT
    InitSystem init;

    init.loadServices();

    init.startRuntime();

    // STARTUP
    Startup startup;

    startup.bootSequence();

    startup.loadDesktop();

    // BOOT
    Boot boot;

    boot.start();

    Splash splash;

    splash.show();

    // FILESYSTEM
    FileSystem filesystem;

    filesystem.checkDisks();

    filesystem.mount();

    // PACKAGE SYSTEM
    PackageManager packages;

    packages.init();

    packages.loadPackages();

    // SERVICES
    ServiceManager services;

    services.startServices();

    // LOGIN
    LoginManager login;

    login.login();

    // SESSION
    SessionManager session;

    session.startSession();

    // DISPLAY
    DisplayServer display;

    display.init();

    // COMPOSITOR
    Compositor compositor;

    compositor.init();

    // RENDERER
    Renderer renderer;

    renderer.init();

    // SDL
    SDLBackend sdl;

    sdl.init();

    sdl.createWindow();

    // SURFACE
    SurfaceManager surface;

    surface.createSurface();

    // GPU
    GPURenderer gpu;

    gpu.initGPU();

    // ANIMATION
    Animations animations;

    animations.fadeIn();

    // THEME
    ThemeEngine theme;

    theme.loadTheme();

    // FONT
    FontRenderer fonts;

    fonts.loadFonts();

    fonts.renderText();

    // WALLPAPER
    Wallpaper wallpaper;

    wallpaper.load();

    wallpaper.render();

    WallpaperEngine wallpaperEngine;

    wallpaperEngine.loadWallpaper();

    wallpaperEngine.animateWallpaper();

    // WIDGETS
    WidgetPanel panel;

    panel.load();

    panel.render();

    // CURSOR
    Cursor cursor;

    cursor.move(200,150);

    cursor.render();

    // INPUT
    InputManager input;

    input.init();

    Keyboard keyboard;

    keyboard.listen();

    Mouse mouse;

    mouse.track();

    input.pollEvents();

    // NETWORK
    NetworkManager network;

    network.init();

    network.connect();

    WiFi wifi;

    wifi.scan();

    wifi.connect();

    wifi.status();

    Ethernet ethernet;

    ethernet.detectCable();

    ethernet.connect();

    DNS dns;

    dns.resolve();

    // AUDIO
    SoundDriver soundDriver;

    soundDriver.detect();

    soundDriver.initialize();

    AudioServer audioServer;

    audioServer.start();

    Mixer mixer;

    mixer.load();

    mixer.volume(80);

    Media media;

    media.play();

    // SYSTEM
    PowerManager power;

    power.init();

    power.batteryStatus();

    LockScreen lockScreen;

    lockScreen.unlock();

    SystemMonitor monitor;

    monitor.cpuUsage();

    monitor.ramUsage();

    monitor.gpuUsage();

    // PROCESS
    ProcessManager process;

    process.init();

    // DESKTOP
    DesktopShell desktop;

    desktop.load();

    Notifications notifications;

    notifications.push(
        "Welcome To Kaizor OS"
    );

    NotificationCenter notificationCenter;

    notificationCenter.open();

    notificationCenter.push(
        "System Ready"
    );

    notificationCenter.push(
        "Network Connected"
    );

    notificationCenter.push(
        "Audio Ready"
    );

    // LAUNCHER
    Launcher launcher;

    launcher.init();

    launcher.open();

    // TASKBAR
    Taskbar taskbar;

    taskbar.init();

    taskbar.render();

    // DOCK
    Dock dock;

    dock.init();

    dock.render();

    DockRuntime dockRuntime;

    dockRuntime.loadApps();

    dockRuntime.render();

    // WORKSPACE
    Workspace workspace;

    workspace.init();

    workspace.switchWorkspace(1);

    // START MENU
    StartMenu startMenu;

    startMenu.open();

    startMenu.render();

    // ICONS
    DesktopIcon icon1(
        "Browser",
        50,
        50
    );

    DesktopIcon icon2(
        "Files",
        50,
        140
    );

    icon1.render();

    icon2.render();

    // APP LAUNCH
    AppLauncher launcherRuntime;

    launcherRuntime.launch(
        "Browser"
    );

    // BUTTON
    Button startButton(
        "Start",
        10,
        678,
        120,
        34
    );

    startButton.render();

    startButton.click();

    // APPS
    FileManager files;

    files.open();

    process.startProcess(
        "File Manager"
    );

    Settings settings;

    settings.open();

    process.startProcess(
        "Settings"
    );

    Terminal terminal;

    terminal.open();

    process.startProcess(
        "Terminal"
    );

    // WINDOWS
    WindowManager wm;

    wm.createWindow(
        1,
        "Files",
        150,
        120,
        450,
        320
    );

    wm.createWindow(
        2,
        "Browser",
        420,
        220,
        520,
        360
    );

    wm.focusWindow(2);

    wm.moveWindow(
        1,
        180,
        150
    );

    wm.renderWindows();

    // COMPOSITOR
    compositor.compose();

    // DISPLAY
    display.refresh();

    cout << "[KAIZOR READY]"
         << endl;

    // GUI LOOP
    sdl.eventLoop();

    // SHUTDOWN
    media.stop();

    audioServer.stop();

    power.shutdown();

    network.disconnect();

    sdl.shutdown();

    return 0;
}
