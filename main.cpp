#include <iostream>

#include "graphics/renderer.h"
#include "graphics/display_server.h"
#include "graphics/compositor.h"

#include "desktop/desktop_shell.h"
#include "desktop/launcher.h"

#include "window_manager/window_manager.h"
#include "window_manager/taskbar.h"
#include "window_manager/dock.h"

#include "input/input_manager.h"
#include "input/keyboard.h"
#include "input/mouse.h"

using namespace std;

int main() {

    cout << "=================================="
         << endl;

    cout << "          KAIZOR OS"
         << endl;

    cout << "=================================="
         << endl;

    // DISPLAY
    DisplayServer display;

    display.init();

    // COMPOSITOR
    Compositor compositor;

    compositor.init();

    // RENDERER
    Renderer renderer;

    renderer.init();

    // INPUT
    InputManager input;

    input.init();

    Keyboard keyboard;

    keyboard.listen();

    Mouse mouse;

    mouse.track();

    input.pollEvents();

    // DESKTOP
    DesktopShell desktop;

    desktop.load();

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

    // COMPOSITOR
    compositor.compose();

    // RENDERER
    renderer.drawFrame();

    // DISPLAY
    display.refresh();

    cout << "[KAIZOR READY]"
         << endl;

    return 0;
}
