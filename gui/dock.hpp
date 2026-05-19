#pragma once

#include <SDL2/SDL_ttf.h>
#include "../graphics/renderer.hpp"

struct DockItem {
    const char* name;
    Color       iconColor;
    Color       glowColor;
    bool        running;
    int         appType;
};

class Dock {
private:
    int mouseX, mouseY;
    int hoveredItem;

    static const int ITEM_COUNT = 6;
    DockItem items[ITEM_COUNT];

public:
    Dock();

    void setMousePos(int mx, int my);
    int  getClickedApp(int mx, int my);

    void render(Renderer& renderer, TTF_Font* font, TTF_Font* smallFont);

    static constexpr int HEIGHT     = 88;
    static constexpr int ICON_SIZE  = 54;
    static constexpr int ICON_GAP   = 10;
    static constexpr int DOCK_PADX  = 20;
    static constexpr int DOCK_PADY  = 10;
};
