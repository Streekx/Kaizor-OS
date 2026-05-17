#include <iostream>

#include "graphics/renderer.h"

#include "desktop/desktop_shell.h"

#include "window_manager/window_manager.h"

#include "graphics/display_server.h"

#include "graphics/compositor.h"

using namespace std;

int main() {

    cout << "=========== KAIZOR OS ===========" 
         << endl;

    Renderer renderer;

    renderer.init();

    DesktopShell desktop;

    desktop.load();

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

    renderer.drawFrame();

    cout << "[KAIZOR READY]"
         << endl;

    return 0;
}
