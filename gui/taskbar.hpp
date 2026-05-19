#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../graphics/renderer.hpp"

class Taskbar {
private:
    int mouseX, mouseY;

public:
    Taskbar();

    void setMousePos(int mx, int my);

    // Returns true if the notification bell was clicked
    bool handleEvent(SDL_Event& e, int screenW = 1280);

    void render(Renderer& renderer, TTF_Font* font, TTF_Font* smallFont,
                const char* activeWindowTitle,
                int unreadNotifCount = 0);

    static constexpr int HEIGHT = 46;
    static constexpr int Y      = 0;
};
