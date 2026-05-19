#include "boot_manager.hpp"
#include <iostream>

static const char* FONT_PATH = "assets/fonts/kaizor.ttf";

BootManager::BootManager(Renderer& r, int w, int h)
    : renderer(r), screenW(w), screenH(h),
      phase(BootPhase::SPLASH), running(true),
      fontXL(nullptr), fontLarge(nullptr), fontMedium(nullptr), fontSmall(nullptr),
      splash(nullptr), loginMgr(nullptr), sessionMgr(nullptr), lockScreen(nullptr),
      desktop(nullptr), wm(nullptr), taskbar(nullptr), dock(nullptr),
      notifMgr(nullptr), notifTimer(0.0f),
      mouseX(0), mouseY(0)
{}

BootManager::~BootManager() { shutdown(); }

bool BootManager::loadFonts() {
    fontXL     = TTF_OpenFont(FONT_PATH, 52);
    fontLarge  = TTF_OpenFont(FONT_PATH, 28);
    fontMedium = TTF_OpenFont(FONT_PATH, 16);
    fontSmall  = TTF_OpenFont(FONT_PATH, 12);

    if (!fontXL || !fontLarge || !fontMedium || !fontSmall) {
        std::cout << "[BOOT] Font load failed: " << TTF_GetError() << std::endl;
        return false;
    }
    std::cout << "[BOOT] Fonts loaded." << std::endl;
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

    desktop = new DesktopShell();
    desktop->wallpaper.loadWallpaper(renderer.getSDLRenderer());

    wm = new WindowManager();
    wm->createWindow(1, "Files",    AppType::FILES,    80,  60, 860, 570);
    wm->createWindow(2, "Terminal", AppType::TERMINAL, 180, 100, 760, 480);

    taskbar  = new Taskbar();
    dock     = new Dock();
    notifMgr = new NotificationManager();
    notifMgr->seedDemoNotifications();

    phase = BootPhase::SPLASH;
    std::cout << "[BOOT] Boot manager initialized." << std::endl;
    return true;
}

void BootManager::advanceTo(BootPhase next) {
    phase = next;
    std::cout << "[BOOT] Phase -> " << (int)next << std::endl;
}

// ── App type map ──────────────────────────────────────────────────────────────
static const struct {
    int dockVal; AppType type; const char* title; int w; int h;
} APP_MAP[] = {
    { 2, AppType::FILES,        "Files",        860, 570 },
    { 1, AppType::TERMINAL,     "Terminal",     760, 480 },
    { 4, AppType::SETTINGS,     "Settings",     900, 560 },
    { 5, AppType::NOTES,        "Notes",        820, 520 },
    { 6, AppType::CALENDAR,     "Calendar",     860, 560 },
    { 3, AppType::TASK_MANAGER, "Task Manager", 920, 580 },
};
static const int APP_MAP_COUNT = 6;

// ── Periodic demo notifications ───────────────────────────────────────────────
static const struct {
    float       delay;
    const char* app;
    const char* title;
    const char* body;
    NotifCategory cat;
    Color       col;
} TIMED_NOTIFS[] = {
    { 18.0f, "Messages",     "Alex",
      "Hey, the build looks great! Nice animations.",
      NotifCategory::MESSAGE, Color(255, 80, 150) },
    { 35.0f, "Terminal",     "Build complete",
      "kaizor compiled with 0 errors, 8 warnings.",
      NotifCategory::APP,    Color(70, 200, 120) },
    { 52.0f, "Update Manager","Security update",
      "Critical patch KB2026-05 is ready to install.",
      NotifCategory::UPDATE, Color(80, 140, 255) },
    { 70.0f, "File Manager", "Download finished",
      "KaizorDE-wallpapers.zip (48 MB) saved.",
      NotifCategory::DOWNLOAD,Color(70, 210, 245) },
    { 88.0f, "Security",     "Login attempt",
      "Failed login from 192.168.1.254 blocked.",
      NotifCategory::ALERT,  Color(255, 120, 50) },
    { 108.0f,"Kaizor OS",    "Low storage",
      "System drive has less than 10 GB free.",
      NotifCategory::SYSTEM, Color(255, 190, 60) },
    { 130.0f,"Messages",     "Jamie",
      "The calendar invite is sent — see you at 3pm!",
      NotifCategory::MESSAGE, Color(255, 80, 150) },
    { 155.0f,"Kaizor OS",    "Backup complete",
      "System snapshot saved to /var/backup.",
      NotifCategory::SYSTEM, Color(80, 200, 160) },
};
static const int TIMED_COUNT = 8;
static bool s_timedSent[8] = {};

void BootManager::handleEvent(SDL_Event& e) {
    if (e.type == SDL_QUIT) { running = false; return; }
    if (e.type == SDL_MOUSEMOTION) { mouseX = e.motion.x; mouseY = e.motion.y; }

    switch (phase) {
    case BootPhase::SPLASH:
        break;

    case BootPhase::LOGIN:
        loginMgr->setMousePos(mouseX, mouseY);
        loginMgr->handleEvent(e);
        break;

    case BootPhase::SESSION_INIT:
        break;

    case BootPhase::DESKTOP: {
        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_ESCAPE) {
                if (desktop->launcher.isOpen()) desktop->launcher.close();
                else running = false;
                return;
            }
            if (e.key.keysym.sym == SDLK_l &&
                (SDL_GetModState() & (KMOD_LGUI|KMOD_RGUI|KMOD_LCTRL))) {
                lockScreen->lock();
                advanceTo(BootPhase::LOCK_SCREEN);
                return;
            }
            if (e.key.keysym.sym == SDLK_F1 ||
                (e.key.keysym.sym == SDLK_SPACE &&
                 (SDL_GetModState() & KMOD_LCTRL))) {
                desktop->launcher.toggle();
                return;
            }
            // Ctrl+N — push a test notification
            if (e.key.keysym.sym == SDLK_n &&
                (SDL_GetModState() & KMOD_LCTRL)) {
                notifMgr->push("Test", "Notification triggered",
                    "You pressed Ctrl+N to test the notification system.",
                    NotifCategory::APP, Color(160, 110, 255));
                return;
            }
        }

        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            int mx2 = e.button.x, my2 = e.button.y;

            // Taskbar K button
            if (mx2 >= 3 && mx2 <= 45 && my2 >= 2 && my2 <= Taskbar::HEIGHT - 2) {
                desktop->launcher.toggle();
                return;
            }

            // Dock click → open/focus app window
            int clickedApp = dock->getClickedApp(mx2, my2);
            if (clickedApp >= 0) {
                for (int i = 0; i < APP_MAP_COUNT; i++) {
                    if (APP_MAP[i].dockVal == clickedApp) {
                        int ox = 60 + (clickedApp % 4) * 40;
                        int oy = 50 + (clickedApp % 3) * 30;
                        wm->focusOrCreate(clickedApp + 100,
                            APP_MAP[i].title, APP_MAP[i].type,
                            ox, oy, APP_MAP[i].w, APP_MAP[i].h);
                        desktop->launcher.close();

                        // Notify when opening an app for the first time
                        notifMgr->push(APP_MAP[i].title,
                            std::string(APP_MAP[i].title) + " opened",
                            std::string("Launching ") + APP_MAP[i].title + ".",
                            NotifCategory::APP,
                            Color(80, 140, 255));
                        return;
                    }
                }
            }
        }

        // Notification bell click is handled inside taskbar handleEvent
        notifMgr->setMousePos(mouseX, mouseY);
        if (taskbar->handleEvent(e)) {
            notifMgr->toggleHistory();
            return;
        }
        notifMgr->handleEvent(e);

        desktop->setMousePos(mouseX, mouseY);
        taskbar->setMousePos(mouseX, mouseY);
        dock->setMousePos(mouseX, mouseY);
        desktop->handleEvent(e);
        wm->handleEvent(e);
        break;
    }

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
        if (splash->finished()) advanceTo(BootPhase::LOGIN);
        break;

    case BootPhase::LOGIN:
        loginMgr->update(dt);
        if (loginMgr->isComplete()) advanceTo(BootPhase::SESSION_INIT);
        break;

    case BootPhase::SESSION_INIT:
        sessionMgr->update(dt);
        if (sessionMgr->isComplete()) advanceTo(BootPhase::DESKTOP);
        break;

    case BootPhase::DESKTOP:
        wm->update();
        desktop->update();
        notifMgr->update(dt);

        // Periodic timed notifications
        notifTimer += dt;
        for (int i = 0; i < TIMED_COUNT; i++) {
            if (!s_timedSent[i] && notifTimer >= TIMED_NOTIFS[i].delay) {
                s_timedSent[i] = true;
                notifMgr->push(TIMED_NOTIFS[i].app,
                               TIMED_NOTIFS[i].title,
                               TIMED_NOTIFS[i].body,
                               TIMED_NOTIFS[i].cat,
                               TIMED_NOTIFS[i].col);
            }
        }
        break;

    case BootPhase::LOCK_SCREEN:
        lockScreen->update(dt);
        if (lockScreen->isUnlocked()) advanceTo(BootPhase::DESKTOP);
        break;
    }
}

void BootManager::renderDesktop() {
    desktop->render(renderer, fontMedium, fontSmall);
    wm->render(renderer, fontMedium, fontSmall);
    taskbar->render(renderer, fontMedium, fontSmall,
                    wm->getFocusedTitle(),
                    notifMgr->unreadCount());
    dock->render(renderer, fontMedium, fontSmall);
    notifMgr->render(renderer, fontMedium, fontSmall);
}

void BootManager::render() {
    switch (phase) {
    case BootPhase::SPLASH:       splash->render(renderer);      break;
    case BootPhase::LOGIN:        loginMgr->render(renderer);    break;
    case BootPhase::SESSION_INIT: sessionMgr->render(renderer);  break;
    case BootPhase::DESKTOP:      renderDesktop();                break;
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
    delete notifMgr;    notifMgr   = nullptr;

    if (fontXL)     { TTF_CloseFont(fontXL);     fontXL     = nullptr; }
    if (fontLarge)  { TTF_CloseFont(fontLarge);  fontLarge  = nullptr; }
    if (fontMedium) { TTF_CloseFont(fontMedium); fontMedium = nullptr; }
    if (fontSmall)  { TTF_CloseFont(fontSmall);  fontSmall  = nullptr; }
}

bool BootManager::isRunning() const { return running; }
