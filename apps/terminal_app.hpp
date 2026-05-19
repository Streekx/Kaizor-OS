#pragma once
#include <SDL2/SDL_ttf.h>
#include "../graphics/renderer.hpp"

class TerminalApp {
private:
    int frame;
public:
    TerminalApp();
    void update();
    void render(Renderer& r, TTF_Font* font, TTF_Font* smallFont,
                int x, int y, int w, int h);
};
