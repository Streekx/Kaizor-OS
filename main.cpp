#include <iostream>

#include "boot/boot.hpp"

#include "core/filesystem.hpp"
#include "core/package_manager.hpp"
#include "core/process_manager.hpp"
#include "core/service_manager.hpp"

#include "graphics/display_server.hpp"
#include "graphics/renderer.hpp"
#include "graphics/compositor.hpp"

#include "gui/gui_renderer.hpp"
#include "gui/blur_engine.hpp"
#include "gui/animation_engine.hpp"
#include "gui/transparency.hpp"

#include "desktop/desktop_shell.hpp"
#include "desktop/dock.hpp"
#include "desktop/taskbar.hpp"
#include "desktop/notifications.hpp"

#include "window_manager/window_manager.hpp"
#include "window_manager/window.hpp"

#include "input/input_manager.hpp"
#include "input/keyboard.hpp"
#include "input/mouse.hpp"

#include "audio/audio_server.hpp"
#include "audio/mixer.hpp"
#include "audio/media.hpp"

#include "networking/network_manager.hpp"
#include "networking/wifi.hpp"
#include "networking/ethernet.hpp"

#include "system/power_manager.hpp"

using namespace std;

int main() {

    cout << "======================================" << endl;
    cout << "         KAIZOR OS STARTUP            " << endl;
    cout << "======================================" << endl;

    /* =========================
       BOOT
       ========================= */

    Boot boot;
    boot.start();

    /* =========================
       CORE
       ========================= */

    FileSystem filesystem;
    filesystem.mount();

    PackageManager pkg;
    pkg.init();

    ServiceManager services;
    services.startServices();

    ProcessManager process;
    process.init();

    /* =========================
       DISPLAY
       ========================= */

    DisplayServer display;
    display.init();

    Renderer renderer;
    renderer.init();

    Compositor compositor;
    compositor.init();

    /* =========================
       GUI
       ========================= */

    GUIRenderer gui;
    gui.initialize();

    BlurEngine blur;
    blur.initialize();

    Transparency transparency;
    transparency.enable();

    AnimationEngine animation;
    animation.start();

    /* =========================
       INPUT
       ========================= */

    InputManager input;
    input.init();

    Keyboard keyboard;
    keyboard.listen();

    Mouse mouse;
    mouse.track();

    /* =========================
       AUDIO
       ========================= */

    AudioServer audio;
    audio.start();

    Mixer mixer;
    mixer.load();

    Media media;
    media.play();

    /* =========================
       NETWORK
       ========================= */

    NetworkManager network;
    network.init();

    WiFi wifi;
    wifi.scan();

    Ethernet ethernet;
    ethernet.detectCable();

    /* =========================
       DESKTOP
       ========================= */

    DesktopShell desktop;
    desktop.load();

    Dock dock;
    dock.init();

    Taskbar taskbar;
    taskbar.init();

    Notifications notifications;
    notifications.push(
        "Welcome To Kaizor OS"
    );

    /* =========================
       WINDOW MANAGER
       ========================= */

    WindowManager wm;

    wm.createWindow(
        1,
        "Files",
        120,
        120,
        500,
        320
    );

    wm.createWindow(
        2,
        "Browser",
        300,
        180,
        700,
        450
    );

    wm.createWindow(
        3,
        "Settings",
        220,
        150,
        420,
        300
    );

    wm.focusWindow(2);

    wm.renderWindows();

    /* =========================
       FINAL RENDER
       ========================= */

    renderer.drawFrame();

    compositor.compose();

    display.refresh();

    cout << endl;
    cout << "[KAIZOR OS READY]" << endl;
    cout << endl;

    return 0;
}
