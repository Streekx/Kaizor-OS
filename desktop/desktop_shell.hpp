#pragma once

#include <SDL2/SDL_ttf.h>
#include "../graphics/renderer.hpp"
#include "wallpaper_engine.hpp"
#include "desktop_icons.hpp"

class DesktopShell {
public:
    WallpaperEngine wallpaper;
    DesktopIcons    icons;

    DesktopShell();

    void setMousePos(int mx, int my);
    void render(Renderer& renderer, TTF_Font* font);
};
