#include "window_manager.hpp"
#include "../gui/ui_theme.hpp"
#include "../gui/taskbar.hpp"
#include "../gui/dock.hpp"
#include "../apps/terminal_app.hpp"
#include "../apps/browser_app.hpp"
#include "../apps/file_manager_app.hpp"
#include "../apps/settings_app.hpp"
#include <cmath>

// ─── Static app instances ──────────────────────────────────────────────────────
static TerminalApp  s_terminal;
static BrowserApp   s_browser;
static FileManagerApp s_files;
static SettingsApp  s_settings;

// ─── Window ───────────────────────────────────────────────────────────────────

Window::Window(int id_, const std::string& title_, AppType type_,
               int x_, int y_, int w_, int h_)
    : id(id_), title(title_), appType(type_),
      x(x_), y(y_), width(w_), height(h_),
      savedX(x_), savedY(y_), savedW(w_), savedH(h_),
      focused(false), minimized(false), maximized(false), pendingClose(false)
{}

bool Window::contains(int mx, int my) const {
    return !minimized && mx >= x && mx < x + width && my >= y && my < y + height;
}
bool Window::titlebarContains(int mx, int my) const {
    return !minimized && mx >= x && mx < x + width && my >= y && my < y + TITLEBAR_H;
}
bool Window::closeContains(int mx, int my) const {
    int bx = x + 21, by = y + TITLEBAR_H / 2;
    int dx = mx - bx, dy = my - by;
    return dx * dx + dy * dy <= 8 * 8;
}
bool Window::minContains(int mx, int my) const {
    int bx = x + 41, by = y + TITLEBAR_H / 2;
    int dx = mx - bx, dy = my - by;
    return dx * dx + dy * dy <= 8 * 8;
}
bool Window::maxContains(int mx, int my) const {
    int bx = x + 61, by = y + TITLEBAR_H / 2;
    int dx = mx - bx, dy = my - by;
    return dx * dx + dy * dy <= 8 * 8;
}
SDL_Rect Window::getContentRect() const {
    return {x, y + TITLEBAR_H, width, height - TITLEBAR_H};
}

// ─── WindowManager ────────────────────────────────────────────────────────────

WindowManager::WindowManager()
    : isDragging(false), dragIdx(-1), dragOfsX(0), dragOfsY(0),
      mouseX(0), mouseY(0)
{}

void WindowManager::createWindow(int id, const std::string& title, AppType type,
                                  int x, int y, int w, int h) {
    for (auto& win : windows) win.focused = false;
    windows.emplace_back(id, title, type, x, y, w, h);
    windows.back().focused = true;
}

void WindowManager::handleEvent(SDL_Event& e) {
    if (e.type == SDL_MOUSEMOTION) {
        mouseX = e.motion.x;
        mouseY = e.motion.y;

        if (isDragging && dragIdx >= 0 && dragIdx < (int)windows.size()) {
            Window& win = windows[dragIdx];
            int nx = e.motion.x - dragOfsX;
            int ny = e.motion.y - dragOfsY;
            ny = std::max(Taskbar::HEIGHT, ny);
            win.x = nx;
            win.y = ny;
        }
        return;
    }

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int mx = e.button.x;
        int my = e.button.y;

        for (int i = (int)windows.size() - 1; i >= 0; i--) {
            if (!windows[i].contains(mx, my)) continue;

            // Bring to front
            if (i != (int)windows.size() - 1) {
                Window tmp = windows[i];
                windows.erase(windows.begin() + i);
                windows.push_back(tmp);
                i = (int)windows.size() - 1;
            }
            for (auto& w : windows) w.focused = false;
            windows.back().focused = true;
            Window& fw = windows.back();

            if (fw.closeContains(mx, my)) { fw.pendingClose = true; return; }

            if (fw.minContains(mx, my)) {
                fw.minimized = !fw.minimized;
                return;
            }
            if (fw.maxContains(mx, my)) {
                if (fw.maximized) {
                    fw.x = fw.savedX; fw.y = fw.savedY;
                    fw.width = fw.savedW; fw.height = fw.savedH;
                    fw.maximized = false;
                } else {
                    fw.savedX = fw.x; fw.savedY = fw.y;
                    fw.savedW = fw.width; fw.savedH = fw.height;
                    fw.x = 0;
                    fw.y = Taskbar::HEIGHT;
                    fw.width  = 1280;
                    fw.height = 720 - Taskbar::HEIGHT - Dock::HEIGHT - 4;
                    fw.maximized = true;
                }
                return;
            }
            if (fw.titlebarContains(mx, my) && !fw.maximized) {
                isDragging = true;
                dragIdx    = (int)windows.size() - 1;
                dragOfsX   = mx - fw.x;
                dragOfsY   = my - fw.y;
            }
            return;
        }
    }

    if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
        isDragging = false;
        dragIdx    = -1;
    }
}

void WindowManager::update() {
    s_terminal.update();

    windows.erase(
        std::remove_if(windows.begin(), windows.end(),
                       [](const Window& w){ return w.pendingClose; }),
        windows.end());
}

// ─── Chrome rendering ─────────────────────────────────────────────────────────

void WindowManager::drawWindowChrome(Renderer& r, TTF_Font* font, TTF_Font* /*smallFont*/,
                                      const Window& win) const {
    const int TH = Window::TITLEBAR_H;
    bool hC = win.closeContains(mouseX, mouseY);
    bool hM = win.minContains(mouseX, mouseY);
    bool hX = win.maxContains(mouseX, mouseY);

    // ─── Multi-layer drop shadow ──────────────────────────────
    r.drawRoundedRect(win.x + 12, win.y + 14, win.width, win.height, 14,
                      Color(0,0,0, 80));
    r.drawRoundedRect(win.x + 6,  win.y + 8,  win.width, win.height, 14,
                      Color(0,0,0, 55));
    r.drawRoundedRect(win.x + 3,  win.y + 4,  win.width, win.height, 14,
                      Color(0,0,0, 30));

    // ─── Window body ─────────────────────────────────────────
    auto wg = UITheme::windowGrad();
    Color bodyTop = win.focused ? wg.top : UITheme::windowNormal();
    Color bodyBot = win.focused ? wg.bottom : UITheme::windowNormal();
    r.drawRoundedVerticalGradient(win.x, win.y, win.width, win.height, 14,
                                   bodyTop, bodyBot);
    r.drawRoundedOutline(win.x, win.y, win.width, win.height, 14,
                         win.focused ? UITheme::borderMid() : UITheme::borderSubtle());

    // ─── Titlebar ─────────────────────────────────────────────
    auto tg = UITheme::titlebarGrad();
    Color tTop = win.focused ? tg.top : UITheme::titlebarNormal();
    Color tBot = win.focused ? tg.bottom : UITheme::titlebarNormal();
    r.drawRoundedVerticalGradient(win.x, win.y, win.width, TH, 14, tTop, tBot);
    r.drawRect(win.x, win.y + TH / 2, win.width, TH / 2 + 1, tBot);
    r.drawLine(win.x + 1, win.y + TH, win.x + win.width - 1, win.y + TH,
               UITheme::borderSubtle());

    // Top-edge shine
    r.drawLine(win.x + 14, win.y + 1, win.x + win.width - 14, win.y + 1,
               Color(255, 255, 255, win.focused ? 22 : 10));

    // ─── Traffic lights ───────────────────────────────────────
    auto drawBtn = [&](int bx, int by, Color col, Color hov, bool hover) {
        // Shadow
        r.drawFilledCircle(bx + 1, by + 1, 7, Color(0,0,0,60));
        // Button
        r.drawFilledCircle(bx, by, 7, hover ? hov : col);
        // Shine
        r.drawFilledCircle(bx - 2, by - 2, 3, Color(255,255,255, hover ? 50 : 30));
    };

    int btnY = win.y + TH / 2;
    drawBtn(win.x + 21, btnY, UITheme::btnClose(),    Color(255, 70, 62), hC);
    drawBtn(win.x + 41, btnY, UITheme::btnMinimize(), Color(220,165, 28), hM);
    drawBtn(win.x + 61, btnY, UITheme::btnMaximize(), Color( 22,176, 42), hX);

    // Hover symbols
    if (hC) {
        r.drawLine(win.x+17, btnY-3, win.x+25, btnY+3, Color(100,0,0,200));
        r.drawLine(win.x+25, btnY-3, win.x+17, btnY+3, Color(100,0,0,200));
    }
    if (hM) {
        r.drawLine(win.x+36, btnY, win.x+46, btnY, Color(100,60,0,200));
    }
    if (hX) {
        r.drawLine(win.x+57, btnY-3, win.x+65, btnY+3, Color(0,80,0,200));
        r.drawLine(win.x+57, btnY+3, win.x+65, btnY-3, Color(0,80,0,200));
    }

    // ─── Window title ─────────────────────────────────────────
    if (font && !win.title.empty()) {
        Color titleCol = win.focused ? UITheme::textPrimary() : UITheme::textMuted();
        r.drawTextCentered(font, win.title,
                           win.x + 80, win.y, win.width - 160, TH, titleCol);
    }
}

// ─── App content dispatch ─────────────────────────────────────────────────────

void WindowManager::drawAppContent(Renderer& r, TTF_Font* font, TTF_Font* smallFont,
                                    const Window& win) {
    SDL_Rect cr = win.getContentRect();
    if (cr.w <= 0 || cr.h <= 0) return;

    r.pushClip(cr.x, cr.y, cr.w, cr.h);
    r.drawRect(cr.x, cr.y, cr.w, cr.h, Color(12, 16, 30, 255));

    switch (win.appType) {
    case AppType::TERMINAL:
        s_terminal.render(r, font, smallFont, cr.x, cr.y, cr.w, cr.h); break;
    case AppType::FILES:
        s_files.render(r, font, smallFont, cr.x, cr.y, cr.w, cr.h); break;
    case AppType::BROWSER:
        s_browser.render(r, font, smallFont, cr.x, cr.y, cr.w, cr.h); break;
    case AppType::SETTINGS:
        s_settings.render(r, font, smallFont, cr.x, cr.y, cr.w, cr.h); break;
    default:
        if (font) r.drawTextCentered(font, "No content", cr.x, cr.y, cr.w, cr.h,
                                      UITheme::textMuted());
        break;
    }

    r.popClip();
}

// ─── Main render ─────────────────────────────────────────────────────────────

void WindowManager::render(Renderer& r, TTF_Font* font, TTF_Font* smallFont) {
    for (auto& win : windows) {
        if (win.minimized) continue;
        drawWindowChrome(r, font, smallFont, win);
        drawAppContent(r, font, smallFont, win);
    }
}

const char* WindowManager::getFocusedTitle() const {
    for (int i = (int)windows.size() - 1; i >= 0; i--) {
        if (windows[i].focused && !windows[i].minimized)
            return windows[i].title.c_str();
    }
    return "";
}
