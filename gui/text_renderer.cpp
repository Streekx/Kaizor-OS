#include "text_renderer.hpp"
#include <iostream>

void TextRenderer::drawText(
    Renderer& renderer,
    TTF_Font* font,
    const std::string& text,
    int x,
    int y,
    SDL_Color color
) {
    if (font == nullptr) {
        return;
    }

    if (text.empty()) {
        return;
    }

    SDL_Surface* surface = TTF_RenderUTF8_Blended(
        font,
        text.c_str(),
        color
    );

    if (!surface) {
        std::cout << "[TEXT] Render failed: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(
        renderer.getSDLRenderer(),
        surface
    );

    if (!texture) {
        std::cout << "[TEXT] Texture failed: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect rect = {
        x,
        y,
        surface->w,
        surface->h
    };

    SDL_RenderCopy(
        renderer.getSDLRenderer(),
        texture,
        nullptr,
        &rect
    );

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}
