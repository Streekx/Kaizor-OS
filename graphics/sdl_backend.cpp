#include <iostream>
#include <SDL2/SDL.h>

#include "sdl_backend.h"

using namespace std;

SDL_Window* window = nullptr;

SDL_Renderer* renderer = nullptr;

bool SDLBackend::init() {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {

        cout << "[SDL] Failed To Start"
             << endl;

        return false;
    }

    cout << "[SDL] Initialized"
         << endl;

    return true;
}

void SDLBackend::createWindow() {

    window = SDL_CreateWindow(
        "Kaizor OS",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280,
        720,
        SDL_WINDOW_SHOWN
    );

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    cout << "[SDL] Window Created"
         << endl;
}

void SDLBackend::render() {

    SDL_SetRenderDrawColor(
        renderer,
        20,
        25,
        35,
        255
    );

    SDL_RenderClear(renderer);

    SDL_RenderPresent(renderer);

    cout << "[SDL] Frame Rendered"
         << endl;
}

void SDLBackend::shutdown() {

    SDL_DestroyRenderer(renderer);

    SDL_DestroyWindow(window);

    SDL_Quit();

    cout << "[SDL] Shutdown Complete"
         << endl;
}
