#pragma once

#include <SDL2/SDL_ttf.h>
#include "../graphics/renderer.hpp"

struct RunningApp {
    const char* name;
    bool        active;
};

class Taskbar {
private:
    int mouseX, mouseY;

public:
    Taskbar();

    void setMousePos(int mx, int my);

    void render(Renderer& renderer, TTF_Font* font, TTF_Font* smallFont,
                const char* activeWindowTitle);

    static constexpr int HEIGHT = 46;
    static constexpr int Y      = 0;
};
