#include <SDL2/SDL.h>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {

        cout << "SDL INIT FAILED" << endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Kaizor OS",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280,
        720,
        SDL_WINDOW_SHOWN
    );

    if (!window) {

        cout << "WINDOW FAILED" << endl;
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    bool running = true;

    SDL_Event event;

    while (running) {

        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {

                running = false;
            }
        }

        SDL_SetRenderDrawColor(
            renderer,
            20, 20, 20, 255
        );

        SDL_RenderClear(renderer);

        // TASKBAR
        SDL_Rect taskbar = {
            0, 680, 1280, 40
        };

        SDL_SetRenderDrawColor(
            renderer,
            40, 40, 40, 255
        );

        SDL_RenderFillRect(
            renderer,
            &taskbar
        );

        // WINDOW
        SDL_Rect win = {
            100, 100, 500, 350
        };

        SDL_SetRenderDrawColor(
            renderer,
            70, 120, 255, 255
        );

        SDL_RenderFillRect(
            renderer,
            &win
        );

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);

    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
