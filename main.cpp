#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "graphics/display_server.hpp"
#include "graphics/renderer.hpp"

#include "desktop/desktop_shell.hpp"

#include "window_manager/window_manager.hpp"

#include "gui/taskbar.hpp"
#include "gui/dock.hpp"

int main() {

    if (
        SDL_Init(SDL_INIT_VIDEO) != 0
    ) {

        return -1;
    }

    if (
        !(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)
    ) {

        return -1;
    }

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

    DesktopShell desktop;

    WindowManager wm;

    wm.createWindow(
        1,
        "Files",
        120,
        100,
        420,
        300
    );

    wm.createWindow(
        2,
        "Browser",
        340,
        180,
        560,
        360
    );

    Taskbar taskbar;

    Dock dock;

    bool running = true;

    SDL_Event event;

    while (running) {

        while (
            SDL_PollEvent(&event)
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

        /* CLEAR SCREEN */

        SDL_SetRenderDrawColor(
            renderer.getSDLRenderer(),
            10,
            14,
            24,
            255
        );

        SDL_RenderClear(
            renderer.getSDLRenderer()
        );

        /* RENDER DESKTOP */

        desktop.render(
            renderer
        );

        /* WINDOWS */

        wm.render(
            renderer
        );

        /* TASKBAR */

        taskbar.render(
            renderer
        );

        /* DOCK */

        dock.render(
            renderer
        );

        /* PRESENT */

        SDL_RenderPresent(
            renderer.getSDLRenderer()
        );

        SDL_Delay(16);
    }

    IMG_Quit();

    SDL_Quit();

    return 0;
}
