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

#include "window_manager/window_manager.h"
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

using namespace std;

int main() {

    cout << "=================================="
         << endl;

    cout << "          KAIZOR OS"
         << endl;

    cout << "=================================="
         << endl;

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

    // SDL BACKEND
    SDLBackend sdl;

    sdl.init();

    sdl.createWindow();

    // SURFACE
    SurfaceManager surface;

    surface.createSurface();

    // GPU
    GPURenderer gpu;

    gpu.initGPU();

    // ANIMATIONS
    Animations animations;

    animations.fadeIn();

    // THEME
    ThemeEngine theme;

    theme.loadTheme();

    // INPUT
    InputManager input;

    input.init();

    Keyboard keyboard;

    keyboard.listen();

    Mouse mouse;

    mouse.track();

    input.pollEvents();

    // PROCESS MANAGER
    ProcessManager process;

    process.init();

    // DESKTOP
    DesktopShell desktop;

    desktop.load();

    // NOTIFICATIONS
    Notifications notifications;

    notifications.push(
        "Welcome To Kaizor OS"
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

    // WORKSPACE
    Workspace workspace;

    workspace.init();

    workspace.switchWorkspace(1);

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

    // WINDOW MANAGER
    WindowManager wm;

    wm.createWindow(
        1,
        "Files",
        100,
        100,
        500,
        400
    );

    wm.createWindow(
        2,
        "Browser",
        300,
        200,
        700,
        500
    );

    wm.render();

    animations.maximize();

    // COMPOSITOR
    compositor.compose();

    // DISPLAY
    display.refresh();

    cout << "[KAIZOR READY]"
         << endl;

    // REAL GUI LOOP
    sdl.eventLoop();

    // SHUTDOWN
    sdl.shutdown();

    return 0;
}
