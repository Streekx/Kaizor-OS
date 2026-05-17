#include <SDL2/SDL.h>

#include "graphics/display_server.hpp"
#include "graphics/renderer.hpp"

#include "window_manager/window_manager.hpp"

#include "gui/ui_theme.hpp"
#include "gui/taskbar.hpp"
#include "gui/dock.hpp"

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
        120,
        100,
        420,
        300
    );

    wm.createWindow(
        2,
        "Browser",
        320,
        180,
        560,
        360
    );

    wm.createWindow(
        3,
        "Settings",
        240,
        140,
        400,
        280
    );

    Taskbar taskbar;

    Dock dock;

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
            UITheme::wallpaper()
        );

        /* TASKBAR */

        taskbar.render(
            renderer
        );

        /* WINDOWS */

        wm.render(
            renderer
        );

        /* DOCK */

        dock.render(
            renderer
        );

        renderer.present();

        SDL_Delay(16);
    }

    renderer.shutdown();

    display.shutdown();

    return 0;
}
