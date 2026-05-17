#include <iostream>
#include <SDL2/SDL.h>

#include "sdl_backend.h"

using namespace std;

SDL_Window* window = nullptr;

SDL_Renderer* renderer = nullptr;

bool running = true;

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

    // WALLPAPER
    SDL_SetRenderDrawColor(
        renderer,
        25,
        30,
        45,
        255
    );

    SDL_RenderClear(renderer);

    // TASKBAR
    SDL_Rect taskbar = {
        0,
        670,
        1280,
        50
    };

    SDL_SetRenderDrawColor(
        renderer,
        40,
        45,
        60,
        255
    );

    SDL_RenderFillRect(
        renderer,
        &taskbar
    );

    // WINDOW 1
    SDL_Rect win1 = {
        100,
        100,
        400,
        300
    };

    SDL_SetRenderDrawColor(
        renderer,
        60,
        70,
        90,
        255
    );

    SDL_RenderFillRect(
        renderer,
        &win1
    );

    // TITLEBAR
    SDL_Rect title1 = {
        100,
        100,
        400,
        30
    };

    SDL_SetRenderDrawColor(
        renderer,
        90,
        110,
        180,
        255
    );

    SDL_RenderFillRect(
        renderer,
        &title1
    );

    // WINDOW 2
    SDL_Rect win2 = {
        350,
        180,
        500,
        350
    };

    SDL_SetRenderDrawColor(
        renderer,
        70,
        80,
        100,
        255
    );

    SDL_RenderFillRect(
        renderer,
        &win2
    );

    // TITLEBAR 2
    SDL_Rect title2 = {
        350,
        180,
        500,
        30
    };

    SDL_SetRenderDrawColor(
        renderer,
        120,
        100,
        200,
        255
    );

    SDL_RenderFillRect(
        renderer,
        &title2
    );

    SDL_RenderPresent(renderer);
}

void SDLBackend::eventLoop() {

    SDL_Event event;

    while (running) {

        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {

                running = false;
            }

            if (event.type == SDL_MOUSEMOTION) {

                cout << "[MOUSE] Moving"
                     << endl;
            }

            if (event.type == SDL_KEYDOWN) {

                cout << "[KEYBOARD] Key Pressed"
                     << endl;
            }
        }

        render();

        SDL_Delay(16);
    }
}

void SDLBackend::shutdown() {

    SDL_DestroyRenderer(renderer);

    SDL_DestroyWindow(window);

    SDL_Quit();

    cout << "[SDL] Shutdown Complete"
         << endl;
}
