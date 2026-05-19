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

static const char* FONT_PATH = "assets/fonts/kaizor.ttf";
static const int   SCREEN_W  = 1280;
static const int   SCREEN_H  = 720;

int main() {

    // ── SDL Init ─────────────────────────────────────────────
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cout << "[SDL] Init Failed: " << SDL_GetError() << std::endl;
        return 1;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cout << "[SDL_IMAGE] Init Failed: " << IMG_GetError() << std::endl;
        return 1;
    }
    if (TTF_Init() == -1) {
        std::cout << "[SDL_TTF] Init Failed: " << TTF_GetError() << std::endl;
        return 1;
    }

    // ── Display ───────────────────────────────────────────────
    DisplayServer display;
    if (!display.initialize("Kaizor OS", SCREEN_W, SCREEN_H)) {
        std::cout << "[DISPLAY] Failed" << std::endl;
        return 1;
    }

    // ── Renderer ──────────────────────────────────────────────
    Renderer renderer;
    if (!renderer.initialize(display.getWindow())) {
        std::cout << "[RENDERER] Failed" << std::endl;
        return 1;
    }

    // ── Fonts ─────────────────────────────────────────────────
    TTF_Font* font      = TTF_OpenFont(FONT_PATH, 14);
    TTF_Font* smallFont = TTF_OpenFont(FONT_PATH, 12);

    if (!font) {
        std::cout << "[FONT] Failed to load regular: " << TTF_GetError() << std::endl;
        return 1;
    }
    if (!smallFont) {
        std::cout << "[FONT] Failed to load small: " << TTF_GetError() << std::endl;
        return 1;
    }
    std::cout << "[FONT] Loaded at 14pt and 12pt" << std::endl;

    // ── Desktop ───────────────────────────────────────────────
    DesktopShell desktop;
    desktop.wallpaper.loadWallpaper(renderer.getSDLRenderer());

    // ── Window Manager ────────────────────────────────────────
    WindowManager wm;
    wm.createWindow(1, "Files",    AppType::FILES,    100,  80,  860, 540);
    wm.createWindow(2, "Terminal", AppType::TERMINAL, 160, 100,  780, 480);
    wm.createWindow(3, "Browser",  AppType::BROWSER,  220, 120,  900, 560);
    wm.createWindow(4, "Settings", AppType::SETTINGS, 280, 140,  820, 500);

    // ── GUI Components ────────────────────────────────────────
    Taskbar taskbar;
    Dock    dock;

    // ── Main Loop ─────────────────────────────────────────────
    bool running = true;
    SDL_Event event;
    int mouseX = 0, mouseY = 0;

    while (running) {

        // ── Events ────────────────────────────────────────────
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) running = false;
            }
            wm.handleEvent(event);
        }

        SDL_GetMouseState(&mouseX, &mouseY);

        taskbar.setMousePos(mouseX, mouseY);
        dock.setMousePos(mouseX, mouseY);
        desktop.setMousePos(mouseX, mouseY);

        wm.update();

        // ── Render Pipeline ───────────────────────────────────
        renderer.clear();

        // 1. Wallpaper + desktop background
        desktop.render(renderer, smallFont);

        // 2. Windows (composited, z-ordered)
        wm.render(renderer, font, smallFont);

        // 3. Taskbar (top)
        taskbar.render(renderer, font, smallFont, wm.getFocusedTitle());

        // 4. Dock (bottom)
        dock.render(renderer, font, smallFont);

        renderer.present();

        SDL_Delay(16); // ~60 fps cap
    }

    // ── Shutdown ──────────────────────────────────────────────
    desktop.wallpaper.destroy();

    TTF_CloseFont(font);
    TTF_CloseFont(smallFont);

    renderer.shutdown();
    display.shutdown();

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    std::cout << "[KAIZOR] Clean shutdown." << std::endl;
    return 0;
}
