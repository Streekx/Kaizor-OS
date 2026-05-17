#include <iostream>

#include "desktop_renderer.h"

using namespace std;

void DesktopRenderer::renderWallpaper() {

    cout << "[DESKTOP] Wallpaper Drawn"
         << endl;
}

void DesktopRenderer::renderDock() {

    cout << "[DESKTOP] Dock Drawn"
         << endl;
}

void DesktopRenderer::renderTaskbar() {

    cout << "[DESKTOP] Taskbar Drawn"
         << endl;
}
