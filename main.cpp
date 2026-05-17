#include <SDL2/SDL.h>

#include <iostream>

#include "graphics/display_server.hpp"
#include "graphics/renderer.hpp"

#include "window_manager/window_manager.hpp"

using namespace std;

int main() {

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

    WindowManager wm;

    wm.createWindow(
        1,
        "Files",
        100,
        100,
        420,
        300
    );

    wm.createWindow(
        2,
        "Browser",
        340,
        160,
        560,
        360
    );

    wm.createWindow(
        3,
        "Settings",
        260,
        120,
        420,
        280
    );

    bool running = true;

    SDL_Event event;

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

            wm.handleEvents(
                event
            );
        }

        /* WALLPAPER */

        renderer.clear(
            Color(
                18,
                20,
                30
            )
        );

        /* TASKBAR */

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

        /* WINDOWS */

        wm.render(
            renderer
        );

        renderer.present();

        SDL_Delay(16);
    }

    renderer.shutdown();

    display.shutdown();

    return 0;
}
