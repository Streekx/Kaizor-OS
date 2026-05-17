// ============================================================
// FILE: main.cpp
// ============================================================

#include <iostream>

#include "graphics/renderer.h"
#include "graphics/display_server.h"
#include "graphics/compositor.h"

#include "desktop/desktop_shell.h"

#include "window_manager/window_manager.h"

using namespace std;

int main() {

    cout << "==================================" << endl;
    cout << "          KAIZOR OS               " << endl;
    cout << "==================================" << endl;

    // DISPLAY SERVER
    DisplayServer display;

    display.init();

    // COMPOSITOR
    Compositor compositor;

    compositor.init();

    // RENDERER
    Renderer renderer;

    renderer.init();

    // DESKTOP
    DesktopShell desktop;

    desktop.load();

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

    // DISPLAY REFRESH
    display.refresh();

    cout << "[KAIZOR READY]" << endl;

    return 0;
}
