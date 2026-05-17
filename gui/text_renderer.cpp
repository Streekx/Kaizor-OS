#include "text_renderer.hpp"

void TextRenderer::drawText(
    Renderer& renderer,
    TTF_Font* font,
    const std::string& text,
    int x,
    int y,
    SDL_Color color
) {

    SDL_Surface* surface =
        TTF_RenderUTF8_Blended(
            font,
            text.c_str(),
            color
        );

    SDL_Texture* texture =
        SDL_CreateTextureFromSurface(
            renderer.getSDLRenderer(),
            surface
        );

    SDL_Rect rect = {

        x,
        y,
        surface->w,
        surface->h
    };

    SDL_RenderCopy(
        renderer.getSDLRenderer(),
        texture,
        NULL,
        &rect
    );

    SDL_FreeSurface(surface);

    SDL_DestroyTexture(texture);
}
