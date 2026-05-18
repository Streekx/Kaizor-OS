#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>

#include "graphics/display_server.hpp"
#include "graphics/renderer.hpp"

#include "desktop/desktop_shell.hpp"
#include "window_manager/window_manager.hpp"

#include "gui/taskbar.hpp"
#include "gui/dock.hpp"

#include "gui/font_manager.hpp"
#include "gui/text_renderer.hpp"

int main() {

    // ============================
    // SDL INIT
    // ============================

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "[SDL] Init Failed: " << SDL_GetError() << std::endl;
        return -1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cout << "[SDL_IMAGE] Init Failed: " << IMG_GetError() << std::endl;
        return -1;
    }

    if (TTF_Init() == -1) {
        std::cout << "[SDL_TTF] Init Failed: " << TTF_GetError() << std::endl;
        return -1;
    }

    // ============================
    // DISPLAY SERVER
    // ============================

    DisplayServer display;

    if (!display.initialize("Kaizor OS", 1280, 720)) {
        std::cout << "[DISPLAY] Failed" << std::endl;
        return -1;
    }

    // ============================
    // RENDERER
    // ============================

    Renderer renderer;

    if (!renderer.initialize(display.getWindow())) {
        std::cout << "[RENDERER] Failed" << std::endl;
        return -1;
    }

    // ============================
    // FONT SYSTEM
    // ============================

    FontManager fontManager;

    if (!fontManager.initialize("assets/fonts/kaizor.ttf", 18)) {
        std::cout << "[FONT] Failed to load font: assets/fonts/kaizor.ttf" << std::endl;
        return -1;
    }

    TextRenderer textRenderer;

    // ============================
    // DESKTOP CORE
    // ============================

    DesktopShell desktop;
    desktop.wallpaper.loadWallpaper(
    renderer.getSDLRenderer()
);
    
    WindowManager wm;

    wm.createWindow(1, "Files", 140, 120, 420, 320);
    wm.createWindow(2, "Browser", 360, 200, 560, 380);
    wm.createWindow(3, "Settings", 240, 160, 460, 320);

    Taskbar taskbar;
    Dock dock;

    // ============================
    // MAIN LOOP
    // ============================

    bool running = true;
    SDL_Event event;

    while (running) {

        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {
                running = false;
            }

            wm.handleEvents(event);
        }

        // ============================
        // RENDER PIPELINE
        // ============================

        desktop.render(renderer);

        wm.render(renderer, textRenderer, fontManager.getFont());

        taskbar.render(renderer);
        dock.render(renderer);

        renderer.present();

        SDL_Delay(16);
    }

    // ============================
    // SHUTDOWN
    // ============================

    fontManager.shutdown();

    renderer.shutdown();
    display.shutdown();

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    return 0;
}
