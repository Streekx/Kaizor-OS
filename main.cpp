#include <SDL2/SDL.h>

#include <iostream>

#include "graphics/display_server.hpp"
#include "graphics/renderer.hpp"
#include "graphics/compositor.hpp"

using namespace std;

int main() {

    cout << endl;

    cout
        << "================================="
        << endl;

    cout
        << "      KAIZOR OS GUI START"
        << endl;

    cout
        << "================================="
        << endl;

    DisplayServer display;

    if (
        !display.initialize(
            "Kaizor OS",
            1280,
            720
        )
    ) {

        return -1;
    }

    Renderer renderer;

    if (
        !renderer.initialize(
            display.getWindow()
        )
    ) {

        return -1;
    }

    Compositor compositor;

    bool running = true;

    SDL_Event event;

    cout
        << "[KAIZOR] Desktop Started"
        << endl;

    while (running) {

        while (
            SDL_PollEvent(
                &event
            )
        ) {

            if (
                event.type ==
                SDL_QUIT
            ) {

                running = false;
            }
        }

        /* =========================
           WALLPAPER
           ========================= */

        renderer.clear(
            Color(
                18,
                20,
                30
            )
        );

        /* =========================
           TASKBAR
           ========================= */

        renderer.drawRect(
            0,
            670,
            1280,
            50,
            Color(
                35,
                40,
                55
            )
        );

        /* =========================
           WINDOW 1
           ========================= */

        renderer.drawRect(
            120,
            100,
            420,
            300,
            Color(
                55,
                65,
                90
            )
        );

        /* =========================
           WINDOW 2
           ========================= */

        renderer.drawRect(
            320,
            180,
            520,
            340,
            Color(
                75,
                85,
                120
            )
        );

        compositor.compose();

        renderer.present();

        SDL_Delay(16);
    }

    renderer.shutdown();

    display.shutdown();

    cout
        << "[KAIZOR] Shutdown Complete"
        << endl;

    return 0;
}
