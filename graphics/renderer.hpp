#pragma once

#include <SDL2/SDL.h>

#include "color.hpp"

class Renderer {

private:

    SDL_Renderer* renderer;

public:

    Renderer();

    bool initialize(
        SDL_Window* window
    );

    void clear(
        Color color
    );

    void drawRect(
        int x,
        int y,
        int width,
        int height,
        Color color
    );

    void drawRoundedRect(
        int x,
        int y,
        int width,
        int height,
        int radius,
        Color color
    );

    void present();

    SDL_Renderer* getSDLRenderer();

    void shutdown();
};
