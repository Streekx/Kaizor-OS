#include "boot_manager.hpp"
#include <iostream>

static const char* FONT_PATH = "assets/fonts/kaizor.ttf";

BootManager::BootManager(Renderer& r, int w, int h)
    : renderer(r), screenW(w), screenH(h),
      phase(BootPhase::SPLASH), running(true),
      fontXL(nullptr), fontLarge(nullptr), fontMedium(nullptr), fontSmall(nullptr),
      splash(nullptr), loginMgr(nullptr), sessionMgr(nullptr), lockScreen(nullptr),
      desktop(nullptr), wm(nullptr), taskbar(nullptr), dock(nullptr),
      mouseX(0), mouseY(0)
{}

BootManager::~BootManager() {
    shutdown();
}

bool BootManager::loadFonts() {
    fontXL     = TTF_OpenFont(FONT_PATH, 52);
    fontLarge  = TTF_OpenFont(FONT_PATH, 28);
    fontMedium = TTF_OpenFont(FONT_PATH, 16);
    fontSmall  = TTF_OpenFont(FONT_PATH, 12);

    if (!fontXL || !fontLarge || !fontMedium || !fontSmall) {
        std::cout << "[BOOT] Font load failed: " << TTF_GetError() << std::endl;
        return false;
    }
    std::cout << "[BOOT] Fonts loaded (52/28/16/12pt)" << std::endl;
    return true;
}

bool BootManager::initialize() {
    if (!loadFonts()) return false;

    splash = new SplashScreen(screenW, screenH);
    splash->setFonts(fontLarge, fontMedium, fontSmall);

    loginMgr = new LoginManager(screenW, screenH);
    loginMgr->setFonts(fontLarge, fontMedium, fontSmall);

    sessionMgr = new SessionManager(screenW, screenH);
    sessionMgr->setFonts(fontLarge, fontMedium, fontSmall);

    lockScreen = new LockScreen(screenW, screenH);
    lockScreen->setFonts(fontXL, fontLarge, fontMedium, fontSmall);

    desktop  = new DesktopShell();
    desktop->wallpaper.loadWallpaper(renderer.getSDLRenderer());

    wm = new WindowManager();
    wm->createWindow(1, "Files",    AppType::FILES,    100,  80,  860, 540);
    wm->createWindow(2, "Terminal", AppType::TERMINAL, 160, 100,  780, 480);
    wm->createWindow(3, "Browser",  AppType::BROWSER,  220, 120,  900, 560);
    wm->createWindow(4, "Settings", AppType::SETTINGS, 280, 140,  820, 500);

    taskbar = new Taskbar();
    dock    = new Dock();

    phase = BootPhase::SPLASH;
    std::cout << "[BOOT] Boot manager initialized." << std::endl;
    return true;
}

void BootManager::advanceTo(BootPhase next) {
    phase = next;
    std::cout << "[BOOT] Phase -> " << (int)next << std::endl;
}

void BootManager::handleEvent(SDL_Event& e) {
    if (e.type == SDL_QUIT) {
        running = false;
        return;
    }

    if (e.type == SDL_MOUSEMOTION) {
        mouseX = e.motion.x;
        mouseY = e.motion.y;
    }

    switch (phase) {
        case BootPhase::SPLASH:
            break;

        case BootPhase::LOGIN:
            loginMgr->setMousePos(mouseX, mouseY);
            loginMgr->handleEvent(e);
            break;

        case BootPhase::SESSION_INIT:
            break;

        case BootPhase::DESKTOP:
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    if (desktop->launcher.isOpen()) {
                        desktop->launcher.close();
                    } else {
                        running = false;
                    }
                    return;
                }
                if (e.key.keysym.sym == SDLK_l &&
                    (SDL_GetModState() & KMOD_LGUI ||
                     SDL_GetModState() & KMOD_RGUI ||
                     SDL_GetModState() & KMOD_LCTRL)) {
                    lockScreen->lock();
                    advanceTo(BootPhase::LOCK_SCREEN);
                    return;
                }
                // Ctrl+Space or F1 toggles the app launcher
                if (e.key.keysym.sym == SDLK_F1 ||
                    (e.key.keysym.sym == SDLK_SPACE &&
                     (SDL_GetModState() & KMOD_LCTRL))) {
                    desktop->launcher.toggle();
                    return;
                }
            }
            // Click on the K/Start button in the taskbar opens the launcher
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                int mx2 = e.button.x, my2 = e.button.y;
                if (mx2 >= 3 && mx2 <= 45 && my2 >= 2 && my2 <= 36) {
                    desktop->launcher.toggle();
                    return;
                }
            }
            desktop->setMousePos(mouseX, mouseY);
            taskbar->setMousePos(mouseX, mouseY);
            dock->setMousePos(mouseX, mouseY);
            desktop->handleEvent(e);
            wm->handleEvent(e);
            break;

        case BootPhase::LOCK_SCREEN:
            lockScreen->setMousePos(mouseX, mouseY);
            lockScreen->handleEvent(e);
            break;
    }
}

void BootManager::update(float dt) {
    switch (phase) {
        case BootPhase::SPLASH:
            splash->update();
            if (splash->finished())
                advanceTo(BootPhase::LOGIN);
            break;

        case BootPhase::LOGIN:
            loginMgr->update(dt);
            if (loginMgr->isComplete())
                advanceTo(BootPhase::SESSION_INIT);
            break;

        case BootPhase::SESSION_INIT:
            sessionMgr->update(dt);
            if (sessionMgr->isComplete())
                advanceTo(BootPhase::DESKTOP);
            break;

        case BootPhase::DESKTOP:
            wm->update();
            desktop->update();
            break;

        case BootPhase::LOCK_SCREEN:
            lockScreen->update(dt);
            if (lockScreen->isUnlocked())
                advanceTo(BootPhase::DESKTOP);
            break;
    }
}

void BootManager::renderDesktop() {
    desktop->render(renderer, fontMedium, fontSmall);
    wm->render(renderer, fontMedium, fontSmall);
    taskbar->render(renderer, fontMedium, fontSmall, wm->getFocusedTitle());
    dock->render(renderer, fontMedium, fontSmall);
}

void BootManager::render() {
    switch (phase) {
        case BootPhase::SPLASH:
            splash->render(renderer);
            break;

        case BootPhase::LOGIN:
            loginMgr->render(renderer);
            break;

        case BootPhase::SESSION_INIT:
            sessionMgr->render(renderer);
            break;

        case BootPhase::DESKTOP:
            renderDesktop();
            break;

        case BootPhase::LOCK_SCREEN:
            renderDesktop();
            lockScreen->render(renderer);
            break;
    }
}

void BootManager::shutdown() {
    delete splash;      splash     = nullptr;
    delete loginMgr;    loginMgr   = nullptr;
    delete sessionMgr;  sessionMgr = nullptr;
    delete lockScreen;  lockScreen = nullptr;
    delete desktop;     desktop    = nullptr;
    delete wm;          wm         = nullptr;
    delete taskbar;     taskbar    = nullptr;
    delete dock;        dock       = nullptr;

    if (fontXL)     { TTF_CloseFont(fontXL);     fontXL     = nullptr; }
    if (fontLarge)  { TTF_CloseFont(fontLarge);  fontLarge  = nullptr; }
    if (fontMedium) { TTF_CloseFont(fontMedium); fontMedium = nullptr; }
    if (fontSmall)  { TTF_CloseFont(fontSmall);  fontSmall  = nullptr; }
}

bool BootManager::isRunning() const {
    return running;
}
