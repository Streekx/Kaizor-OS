#include "desktop_shell.hpp"
#include "../gui/ui_theme.hpp"
#include <ctime>

DesktopShell::DesktopShell() {}

void DesktopShell::setMousePos(int mx, int my) {
    icons.setMousePos(mx, my);
}

void DesktopShell::render(Renderer& renderer, TTF_Font* font) {
    wallpaper.render(renderer);
    icons.render(renderer, font);
}
