#include "desktop_shell.hpp"
#include "../graphics/display_server.hpp"

DesktopShell::DesktopShell() {
}

void DesktopShell::render(Renderer& renderer) {

    // Load wallpaper only once (lazy init)
    static bool wallpaperLoaded = false;
    if (!wallpaperLoaded) {
        wallpaper.loadWallpaper(renderer.getSDLRenderer());
        wallpaperLoaded = true;
    }

    wallpaper.render(renderer);
    icons.render(renderer);
}
