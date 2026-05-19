#pragma once

#include <SDL2/SDL_ttf.h>
#include "../graphics/renderer.hpp"
#include "wallpaper_engine.hpp"
#include "desktop_icons.hpp"
#include "app_launcher.hpp"

class DesktopShell {
public:
    WallpaperEngine wallpaper;
    DesktopIcons    icons;
    AppLauncher     launcher;

    DesktopShell();

    void setMousePos(int mx, int my);
    void handleEvent(SDL_Event& e);
    void update();
    void render(Renderer& renderer, TTF_Font* font, TTF_Font* smallFont);
};
