#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "../graphics/renderer.hpp"
#include "splash.h"
#include "login_manager.h"
#include "../core/session_manager.h"
#include "../system/lock_screen.h"
#include "../desktop/desktop_shell.hpp"
#include "../window_manager/window_manager.hpp"
#include "../gui/taskbar.hpp"
#include "../gui/dock.hpp"

enum class BootPhase {
    SPLASH,
    LOGIN,
    SESSION_INIT,
    DESKTOP,
    LOCK_SCREEN
};

class BootManager {
private:
    Renderer&    renderer;
    int          screenW, screenH;
    BootPhase    phase;
    bool         running;

    TTF_Font* fontXL;
    TTF_Font* fontLarge;
    TTF_Font* fontMedium;
    TTF_Font* fontSmall;

    SplashScreen*   splash;
    LoginManager*   loginMgr;
    SessionManager* sessionMgr;
    LockScreen*     lockScreen;

    DesktopShell*  desktop;
    WindowManager* wm;
    Taskbar*       taskbar;
    Dock*          dock;

    int   mouseX, mouseY;

    bool loadFonts();
    void advanceTo(BootPhase next);
    void renderDesktop();
    void updateDesktop(float dt);
    void handleDesktopEvent(SDL_Event& e);

public:
    BootManager(Renderer& r, int w, int h);
    ~BootManager();

    bool initialize();
    void handleEvent(SDL_Event& e);
    void update(float dt);
    void render();
    void shutdown();
    bool isRunning() const;
};
