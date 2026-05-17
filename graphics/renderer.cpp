#include "renderer.hpp"

#include <iostream>

Renderer::Renderer() {

    renderer = nullptr;
}

bool Renderer::initialize(
    SDL_Window* window
) {

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    if (!renderer) {

        std::cout
            << "[RENDERER] Failed"
            << std::endl;

        return false;
    }

    std::cout
        << "[RENDERER] Initialized"
        << std::endl;

    return true;
}

void Renderer::clear(
    Color color
) {

    SDL_SetRenderDrawColor(
        renderer,
        color.r,
        color.g,
        color.b,
        color.a
    );

    SDL_RenderClear(renderer);
}

void Renderer::drawRect(
    int x,
    int y,
    int width,
    int height,
    Color color
) {

    SDL_Rect rect = {

        x,
        y,
        width,
        height
    };

    SDL_SetRenderDrawColor(
        renderer,
        color.r,
        color.g,
        color.b,
        color.a
    );

    SDL_RenderFillRect(
        renderer,
        &rect
    );
}

void Renderer::present() {

    SDL_RenderPresent(renderer);
}

SDL_Renderer* Renderer::getSDLRenderer() {

    return renderer;
}

void Renderer::shutdown() {

    SDL_DestroyRenderer(renderer);

    std::cout
        << "[RENDERER] Shutdown"
        << std::endl;
}
