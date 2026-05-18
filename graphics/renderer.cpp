#include "renderer.hpp"
#include <iostream>

Renderer::Renderer() {
    renderer = nullptr;
}

bool Renderer::initialize(SDL_Window* window) {

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    if (!renderer) {
        std::cout << "[RENDERER] Failed: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    std::cout << "[RENDERER] Initialized" << std::endl;
    return true;
}

void Renderer::clear(Color color) {

    SDL_SetRenderDrawColor(
        renderer,
        color.r,
        color.g,
        color.b,
        color.a
    );

    SDL_RenderClear(renderer);
}

void Renderer::drawRect(int x, int y, int width, int height, Color color) {

    SDL_Rect rect = { x, y, width, height };

    SDL_SetRenderDrawColor(
        renderer,
        color.r,
        color.g,
        color.b,
        color.a
    );

    SDL_RenderFillRect(renderer, &rect);
}

void Renderer::drawRoundedRect(
    int x,
    int y,
    int width,
    int height,
    int radius,
    Color color
) {
    // Center
    drawRect(x + radius, y, width - (radius * 2), height, color);

    // Left
    drawRect(x, y + radius, radius, height - (radius * 2), color);

    // Right
    drawRect(x + width - radius, y + radius, radius, height - (radius * 2), color);

    // Corners (simple square approximation)
    drawRect(x + 2, y + 2, radius, radius, color);
    drawRect(x + width - radius - 2, y + 2, radius, radius, color);
    drawRect(x + 2, y + height - radius - 2, radius, radius, color);
    drawRect(x + width - radius - 2, y + height - radius - 2, radius, radius, color);
}

void Renderer::drawTexture(SDL_Texture* texture, int x, int y, int width, int height) {

    if (!texture) return;

    SDL_Rect dst = { x, y, width, height };
    SDL_RenderCopy(renderer, texture, nullptr, &dst);
}

void Renderer::present() {
    SDL_RenderPresent(renderer);
}

SDL_Renderer* Renderer::getSDLRenderer() {
    return renderer;
}

void Renderer::shutdown() {

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
}
