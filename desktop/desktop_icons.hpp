#pragma once

#include <SDL2/SDL_ttf.h>
#include "../graphics/renderer.hpp"

struct DesktopIcon {
    const char* name;
    Color       color;
    Color       innerColor;
    int         x, y;
};

class DesktopIcons {
private:
    static const int COUNT = 5;
    DesktopIcon icons[COUNT];
    int mouseX, mouseY;
    int hoveredIcon;

public:
    DesktopIcons();

    void setMousePos(int mx, int my);

    void render(Renderer& renderer, TTF_Font* font);
};
