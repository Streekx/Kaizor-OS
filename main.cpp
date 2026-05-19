#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

#include "graphics/display_server.hpp"
#include "graphics/renderer.hpp"
#include "boot/boot_manager.hpp"

static const int SCREEN_W = 1280;
static const int SCREEN_H = 720;

int main() {

    // ── SDL Init ──────────────────────────────────────────────
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cout << "[SDL] Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cout << "[IMG] Init failed: " << IMG_GetError() << std::endl;
        return 1;
    }
    if (TTF_Init() == -1) {
        std::cout << "[TTF] Init failed: " << TTF_GetError() << std::endl;
        return 1;
    }

    // ── Display ───────────────────────────────────────────────
    DisplayServer display;
    if (!display.initialize("Kaizor OS", SCREEN_W, SCREEN_H)) {
        std::cout << "[DISPLAY] Failed to initialize window" << std::endl;
        return 1;
    }

    // ── Renderer ──────────────────────────────────────────────
    Renderer renderer;
    if (!renderer.initialize(display.getWindow())) {
        std::cout << "[RENDERER] Failed to initialize" << std::endl;
        return 1;
    }

    // ── Boot Pipeline ─────────────────────────────────────────
    //   main.cpp -> BootManager
    //                -> SplashScreen      (boot animation)
    //                -> LoginManager      (login screen)
    //                -> SessionManager    (session init)
    //                -> DesktopShell      (desktop env)
    //                -> LockScreen        (lock screen)

    BootManager boot(renderer, SCREEN_W, SCREEN_H);
    if (!boot.initialize()) {
        std::cout << "[BOOT] Initialization failed" << std::endl;
        return 1;
    }

    // ── Main Loop ─────────────────────────────────────────────
    SDL_Event event;
    Uint32    lastTime = SDL_GetTicks();

    while (boot.isRunning()) {
        while (SDL_PollEvent(&event)) {
            boot.handleEvent(event);
        }

        Uint32 now = SDL_GetTicks();
        float  dt  = (float)(now - lastTime) / 1000.0f;
        if (dt > 0.05f) dt = 0.05f;
        lastTime = now;

        boot.update(dt);

        renderer.clear(Color(0, 0, 0));
        boot.render();
        renderer.present();

        SDL_Delay(16);
    }

    // ── Shutdown ──────────────────────────────────────────────
    boot.shutdown();
    renderer.shutdown();
    display.shutdown();

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    std::cout << "[KAIZOR] Clean shutdown." << std::endl;
    return 0;
}
