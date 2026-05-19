#include "desktop_shell.hpp"
#include "../gui/ui_theme.hpp"

DesktopShell::DesktopShell() {}

void DesktopShell::setMousePos(int mx, int my) {
    icons.setMousePos(mx, my);
    launcher.setMousePos(mx, my);
}

void DesktopShell::handleEvent(SDL_Event& e) {
    launcher.handleEvent(e);
}

void DesktopShell::update() {
    launcher.update();
}

void DesktopShell::render(Renderer& renderer, TTF_Font* font, TTF_Font* smallFont) {
    wallpaper.render(renderer);
    icons.render(renderer, font);
    launcher.render(renderer, font, smallFont);
}
