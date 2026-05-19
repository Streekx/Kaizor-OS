#pragma once
#include <SDL2/SDL_ttf.h>
#include "../graphics/renderer.hpp"

class SettingsApp {
public:
    void render(Renderer& r, TTF_Font* font, TTF_Font* smallFont,
                int x, int y, int w, int h);
};
