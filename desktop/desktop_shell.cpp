#include "desktop_shell.hpp"

DesktopShell::DesktopShell() {
}

void DesktopShell::render(
    Renderer& renderer
) {

    wallpaper.render(
        renderer
    );

    icons.render(
        renderer
    );
}
