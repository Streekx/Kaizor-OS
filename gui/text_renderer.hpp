#pragma once

#include <string>

#include <SDL2/SDL_ttf.h>

#include "../graphics/renderer.hpp"

class TextRenderer {

public:

    void drawText(
        Renderer& renderer,
        TTF_Font* font,
        const std::string& text,
        int x,
        int y,
        SDL_Color color
    );
};
