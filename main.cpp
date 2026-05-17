#include <SDL2/SDL.h>

#include "graphics/display_server.hpp"
#include "graphics/renderer.hpp"

#include "window_manager/window_manager.hpp"

#include "gui/ui_theme.hpp"
#include "gui/taskbar.hpp"
#include "gui/dock.hpp"

#include "gui/font_manager.hpp"
#include "gui/text_renderer.hpp"

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

    FontManager fonts;

    fonts.initialize(
        "assets/fonts/Inter-Regular.ttf",
        16
    );

    TextRenderer textRenderer;

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

        renderer.clear(
            UITheme::wallpaper()
        );

        taskbar.render(
            renderer
        );

        wm.render(
            renderer,
            textRenderer,
            fonts.getFont()
        );

        dock.render(
            renderer
        );

        renderer.present();

        SDL_Delay(16);
    }

    fonts.shutdown();

    renderer.shutdown();

    display.shutdown();

    return 0;
}
