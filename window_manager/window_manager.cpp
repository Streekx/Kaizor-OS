#include "window_manager.hpp"
#include "../gui/ui_theme.hpp"
#include "../gui/taskbar.hpp"
#include "../gui/dock.hpp"
#include "../apps/terminal_app.hpp"
#include "../apps/file_manager_app.hpp"
#include "../apps/settings_app.hpp"
#include "../apps/task_manager_app.hpp"
#include <cmath>
#include <cstring>
#include <cstdio>

// ─── Static app instances ──────────────────────────────────────────────────────
static TerminalApp    s_terminal;
static FileManagerApp s_files;
static SettingsApp    s_settings;
static TaskManagerApp s_taskman;

// ─── Window ───────────────────────────────────────────────────────────────────

Window::Window(int id_, const std::string& title_, AppType type_,
               int x_, int y_, int w_, int h_)
    : id(id_), title(title_), appType(type_),
      x(x_), y(y_), width(w_), height(h_),
      savedX(x_), savedY(y_), savedW(w_), savedH(h_),
      focused(false), minimized(false), maximized(false), pendingClose(false)
{}

bool Window::contains(int mx, int my) const {
    return !minimized && mx >= x && mx < x+width && my >= y && my < y+height;
}
bool Window::titlebarContains(int mx, int my) const {
    return !minimized && mx >= x+80 && mx < x+width-10 && my >= y && my < y+TITLEBAR_H;
}
bool Window::closeContains(int mx, int my) const {
    int bx = x+18, by = y+TITLEBAR_H/2;
    int dx = mx-bx, dy = my-by;
    return dx*dx+dy*dy <= 8*8;
}
bool Window::minContains(int mx, int my) const {
    int bx = x+40, by = y+TITLEBAR_H/2;
    int dx = mx-bx, dy = my-by;
    return dx*dx+dy*dy <= 8*8;
}
bool Window::maxContains(int mx, int my) const {
    int bx = x+62, by = y+TITLEBAR_H/2;
    int dx = mx-bx, dy = my-by;
    return dx*dx+dy*dy <= 8*8;
}
bool Window::resizeContains(int mx, int my) const {
    if (minimized || maximized) return false;
    int margin = 8;
    bool right  = (mx >= x+width-margin && mx < x+width);
    bool bottom = (my >= y+height-margin && my < y+height);
    return (right || bottom) && contains(mx, my);
}
SDL_Rect Window::getContentRect() const {
    return { x, y+TITLEBAR_H, width, height-TITLEBAR_H };
}

// ─── WindowManager ────────────────────────────────────────────────────────────

WindowManager::WindowManager()
    : isDragging(false), dragIdx(-1), dragOfsX(0), dragOfsY(0),
      isResizing(false), resizeIdx(-1),
      resizeStartX(0), resizeStartY(0), resizeStartW(0), resizeStartH(0),
      mouseX(0), mouseY(0)
{}

void WindowManager::createWindow(int id, const std::string& title, AppType type,
                                  int x, int y, int w, int h) {
    for (auto& win : windows) win.focused = false;
    windows.emplace_back(id, title, type, x, y, w, h);
    windows.back().focused = true;
}

void WindowManager::focusOrCreate(int id, const std::string& title, AppType type,
                                   int x, int y, int w, int h) {
    // Bring existing window of same type to front
    for (int i = (int)windows.size()-1; i >= 0; i--) {
        if (windows[i].appType == type) {
            Window tmp = windows[i];
            windows.erase(windows.begin()+i);
            windows.push_back(tmp);
            for (auto& win : windows) win.focused = false;
            windows.back().focused = true;
            if (windows.back().minimized)
                windows.back().minimized = false;
            return;
        }
    }
    createWindow(id, title, type, x, y, w, h);
}

bool WindowManager::hasWindowOfType(AppType t) const {
    for (auto& w : windows)
        if (w.appType == t && !w.pendingClose) return true;
    return false;
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
            ny = std::min(ny, 720 - Dock::HEIGHT - Window::TITLEBAR_H - 4);
            win.x = nx;
            win.y = ny;
        }
        if (isResizing && resizeIdx >= 0 && resizeIdx < (int)windows.size()) {
            Window& win = windows[resizeIdx];
            int dx = e.motion.x - resizeStartX;
            int dy = e.motion.y - resizeStartY;
            win.width  = std::max(Window::MIN_W, resizeStartW + dx);
            win.height = std::max(Window::MIN_H, resizeStartH + dy);
        }
        return;
    }

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int mx = e.button.x;
        int my = e.button.y;

        for (int i = (int)windows.size()-1; i >= 0; i--) {
            if (!windows[i].contains(mx, my)) continue;

            // Bring to front
            if (i != (int)windows.size()-1) {
                Window tmp = windows[i];
                windows.erase(windows.begin()+i);
                windows.push_back(tmp);
                i = (int)windows.size()-1;
            }
            for (auto& w : windows) w.focused = false;
            windows.back().focused = true;
            Window& fw = windows.back();

            if (fw.closeContains(mx,my))    { fw.pendingClose = true; return; }
            if (fw.minContains(mx,my))      { fw.minimized = !fw.minimized; return; }
            if (fw.maxContains(mx,my)) {
                if (fw.maximized) {
                    fw.x=fw.savedX; fw.y=fw.savedY;
                    fw.width=fw.savedW; fw.height=fw.savedH;
                    fw.maximized=false;
                } else {
                    fw.savedX=fw.x; fw.savedY=fw.y;
                    fw.savedW=fw.width; fw.savedH=fw.height;
                    fw.x=0; fw.y=Taskbar::HEIGHT;
                    fw.width=1280;
                    fw.height=720-Taskbar::HEIGHT-Dock::HEIGHT-4;
                    fw.maximized=true;
                }
                return;
            }
            if (fw.resizeContains(mx,my) && !fw.maximized) {
                isResizing   = true;
                resizeIdx    = (int)windows.size()-1;
                resizeStartX = mx; resizeStartY = my;
                resizeStartW = fw.width; resizeStartH = fw.height;
                return;
            }
            if (fw.titlebarContains(mx,my) && !fw.maximized) {
                isDragging = true;
                dragIdx    = (int)windows.size()-1;
                dragOfsX   = mx - fw.x;
                dragOfsY   = my - fw.y;
                return;
            }

            // Forward event to app content
            SDL_Rect cr = fw.getContentRect();
            switch (fw.appType) {
            case AppType::FILES:
                s_files.handleEvent(e, cr.x, cr.y, cr.w, cr.h); break;
            case AppType::SETTINGS:
                s_settings.handleEvent(e, cr.x, cr.y, cr.w, cr.h); break;
            case AppType::TASK_MANAGER:
                s_taskman.handleEvent(e, cr.x, cr.y, cr.w, cr.h); break;
            default: break;
            }
            return;
        }
    }

    if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
        isDragging = false; dragIdx = -1;
        isResizing = false; resizeIdx = -1;
    }
}

void WindowManager::update() {
    s_terminal.update();
    s_files.update();
    s_settings.update();
    s_taskman.update();

    windows.erase(
        std::remove_if(windows.begin(), windows.end(),
                       [](const Window& w){ return w.pendingClose; }),
        windows.end());
}

// ─── Chrome ───────────────────────────────────────────────────────────────────

void WindowManager::drawWindowChrome(Renderer& r, TTF_Font* font, TTF_Font* /*sf*/,
                                      const Window& win) const {
    const int TH = Window::TITLEBAR_H;

    bool hC = win.closeContains(mouseX, mouseY);
    bool hM = win.minContains(mouseX, mouseY);
    bool hX = win.maxContains(mouseX, mouseY);

    // Multi-layer drop shadows
    r.drawRoundedRect(win.x+14, win.y+16, win.width, win.height, 16, Color(0,0,0,90));
    r.drawRoundedRect(win.x+7,  win.y+9,  win.width, win.height, 16, Color(0,0,0,60));
    r.drawRoundedRect(win.x+3,  win.y+4,  win.width, win.height, 16, Color(0,0,0,32));

    // Window body
    auto wg = UITheme::windowGrad();
    Color bodyTop = win.focused ? wg.top : UITheme::windowNormal();
    Color bodyBot = win.focused ? wg.bottom : UITheme::windowNormal();
    r.drawRoundedVerticalGradient(win.x, win.y, win.width, win.height, 14, bodyTop, bodyBot);
    r.drawRoundedOutline(win.x, win.y, win.width, win.height, 14,
        win.focused ? UITheme::borderMid() : UITheme::borderSubtle());

    // Titlebar gradient
    auto tg = UITheme::titlebarGrad();
    Color tTop = win.focused ? tg.top : UITheme::titlebarNormal();
    Color tBot = win.focused ? tg.bottom : UITheme::titlebarNormal();
    r.drawRoundedVerticalGradient(win.x, win.y, win.width, TH, 14, tTop, tBot);
    r.drawRect(win.x, win.y+TH/2, win.width, TH/2+1, tBot);
    r.drawLine(win.x+1, win.y+TH, win.x+win.width-1, win.y+TH, UITheme::borderSubtle());

    // Top-edge shine
    r.drawLine(win.x+14, win.y+1, win.x+win.width-14, win.y+1,
        Color(255,255,255, win.focused ? 20 : 8));
    r.drawLine(win.x+14, win.y+2, win.x+win.width-14, win.y+2,
        Color(255,255,255, win.focused ? 8 : 4));

    // Traffic-light buttons
    auto drawBtn = [&](int bx, int by, Color col, Color hov, bool hover, const char* sym) {
        r.drawFilledCircle(bx+1, by+1, 8, Color(0,0,0,55));
        r.drawFilledCircle(bx, by, 8, hover ? hov : col);
        r.drawFilledCircle(bx-2, by-2, 3, Color(255,255,255, hover ? 60 : 35));
        if (hover && sym) {
            // Symbol inside button
            r.drawFilledCircle(bx, by, 4, Color(0,0,0,100));
        }
    };

    int btnY = win.y + TH/2;
    drawBtn(win.x+18, btnY, UITheme::btnClose(),    Color(255,65,58),  hC, "x");
    drawBtn(win.x+40, btnY, UITheme::btnMinimize(), Color(220,160,25), hM, "-");
    drawBtn(win.x+62, btnY, UITheme::btnMaximize(), Color(22,176,42),  hX, "+");

    // Hover symbols drawn on top
    if (hC) {
        r.drawLine(win.x+14, btnY-3, win.x+22, btnY+3, Color(80,0,0,210));
        r.drawLine(win.x+22, btnY-3, win.x+14, btnY+3, Color(80,0,0,210));
    }
    if (hM) {
        r.drawLine(win.x+35, btnY, win.x+45, btnY, Color(80,50,0,210));
    }
    if (hX) {
        if (win.maximized) {
            r.drawLine(win.x+57, btnY-3, win.x+67, btnY+3, Color(0,60,0,210));
            r.drawLine(win.x+57, btnY+3, win.x+67, btnY-3, Color(0,60,0,210));
        } else {
            r.drawRect(win.x+58, btnY-4, 8, 8, Color(0,0,0,0));
            r.drawOutlineRect(win.x+58, btnY-4, 8, 8, Color(0,60,0,210));
        }
    }

    // Window title (centered)
    if (font && !win.title.empty()) {
        Color titleCol = win.focused ? UITheme::textPrimary() : UITheme::textMuted();
        r.drawTextCentered(font, win.title,
            win.x+80, win.y, win.width-160, TH, titleCol);
    }

    // Resize handle indicator (bottom-right corner)
    if (!win.maximized) {
        int rx = win.x+win.width-12, ry = win.y+win.height-12;
        Color rhc = Color(60,75,130,80);
        for (int i=0; i<3; i++) {
            r.drawLine(rx+i*3, ry+8, rx+8, ry+i*3, rhc);
        }
    }
}

// ─── App content ──────────────────────────────────────────────────────────────

static void drawNotesApp(Renderer& r, TTF_Font* hf, TTF_Font* sf, SDL_Rect cr) {
    r.drawRect(cr.x, cr.y, cr.w, cr.h, Color(12,16,30,255));
    // Toolbar
    r.drawVerticalGradient(cr.x, cr.y, cr.w, 44, Color(16,20,44,255), Color(12,16,38,255));
    r.drawLine(cr.x, cr.y+44, cr.x+cr.w, cr.y+44, Color(32,44,80,150));
    if (sf) {
        const char* actions[] = {"New","Delete","Format","Tags","Share"};
        int ax = cr.x+8;
        for (auto& a : actions) {
            int aw = r.measureTextW(sf, a)+20;
            r.drawRoundedRect(ax, cr.y+8, aw, 28, 7, Color(22,28,56,200));
            r.drawRoundedRect(ax, cr.y+8, aw, 28, 7, Color(50,65,110,60));
            r.drawText(sf, a, ax+10, cr.y+8+(28-r.measureTextH(sf,"A"))/2,
                Color(180,195,235,215));
            ax += aw+6;
        }
    }
    // Sidebar
    const int SW=220;
    r.drawVerticalGradient(cr.x, cr.y+44, SW, cr.h-44, Color(14,18,38,255), Color(10,14,32,255));
    r.drawLine(cr.x+SW, cr.y+44, cr.x+SW, cr.y+cr.h, Color(32,44,80,150));
    struct { const char* title; const char* date; } notes[] = {
        {"Meeting Notes","Today, 09:00"},
        {"Project Ideas","Yesterday"},
        {"Shopping List","May 16, 2026"},
        {"Kaizor OS Todo","May 15, 2026"},
        {"Quick Thoughts","May 14, 2026"},
    };
    int ny = cr.y+52;
    for (int i=0; i<5; i++) {
        int ry = ny+i*58;
        bool sel = (i==0);
        if (sel) {
            r.drawRoundedRect(cr.x+5, ry, SW-10, 50, 8, Color(38,54,120,190));
            r.drawRect(cr.x+5, ry, 3, 50, Color(80,140,255,220));
        }
        if (hf) r.drawText(hf, notes[i].title, cr.x+18, ry+8,
            sel ? UITheme::textPrimary() : UITheme::textSecondary());
        if (sf) r.drawText(sf, notes[i].date, cr.x+18, ry+28, UITheme::textMuted());
        r.drawLine(cr.x+10, ry+50, cr.x+SW-10, ry+50, Color(28,36,65,100));
    }
    // Content
    int cx = cr.x+SW+1;
    int cw = cr.w-SW-1;
    r.drawRect(cx, cr.y+44, cw, cr.h-44, Color(10,13,26,255));
    if (hf) r.drawText(hf, "Meeting Notes", cx+24, cr.y+56, UITheme::textPrimary());
    if (sf) {
        r.drawText(sf, "Today, 09:00  —  2 hours ago", cx+24, cr.y+80, UITheme::textMuted());
        r.drawLine(cx+16, cr.y+100, cx+cw-16, cr.y+100, Color(32,44,80,100));
        const char* lines[] = {
            "Discussed roadmap for Q3 2026",
            "• New UI components for the desktop shell",
            "• Performance improvements to the compositor",
            "• File manager enhancements — grid view, tags",
            "",
            "Action items:",
            "• Finish settings app — due Friday",
            "• Update bootloader animation",
            "• Review PR #42 from core team",
        };
        for (int i=0; i<9; i++) {
            if (sf && lines[i][0])
                r.drawText(sf, lines[i], cx+24, cr.y+112+i*22,
                    i==5 ? UITheme::textSecondary() : Color(190,202,235,210));
        }
    }
}

static void drawCalendarApp(Renderer& r, TTF_Font* hf, TTF_Font* sf, SDL_Rect cr) {
    r.drawRect(cr.x, cr.y, cr.w, cr.h, Color(10,13,26,255));
    // Header
    r.drawVerticalGradient(cr.x, cr.y, cr.w, 56, Color(16,20,44,255), Color(12,16,38,255));
    r.drawLine(cr.x, cr.y+56, cr.x+cr.w, cr.y+56, Color(32,44,80,150));
    if (hf) r.drawTextCentered(hf, "May 2026", cr.x, cr.y, cr.w, 56, UITheme::textPrimary());
    // Nav
    r.drawRoundedRect(cr.x+12, cr.y+14, 28, 28, 7, Color(22,28,56,200));
    r.drawRoundedRect(cr.x+cr.w-40, cr.y+14, 28, 28, 7, Color(22,28,56,200));
    if (sf) {
        r.drawTextCentered(sf, "<", cr.x+12, cr.y+14, 28, 28, UITheme::textSecondary());
        r.drawTextCentered(sf, ">", cr.x+cr.w-40, cr.y+14, 28, 28, UITheme::textSecondary());
    }
    // Day headers
    const char* days[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
    int colW = cr.w / 7;
    int daysY = cr.y+56;
    r.drawRect(cr.x, daysY, cr.w, 28, Color(14,18,40,200));
    for (int i=0; i<7; i++) {
        if (sf) r.drawTextCentered(sf, days[i], cr.x+i*colW, daysY, colW, 28,
            i==0||i==6 ? Color(255,100,100,180) : UITheme::textMuted());
    }
    // Calendar grid
    int rowH = (cr.h-56-28) / 6;
    int dateNum = 1;
    for (int row=0; row<6; row++) {
        for (int col=0; col<7; col++) {
            if (row==0 && col<3) continue; // May starts Thursday(4)... adjust
            if (dateNum > 31) break;
            int cx2 = cr.x + col*colW;
            int cy2 = cr.y + 56 + 28 + row*rowH;
            bool isToday = (dateNum == 19);
            bool isWknd  = (col==0||col==6);
            r.drawLine(cx2, cy2, cx2+colW, cy2, Color(28,36,65,80));
            if (isToday) {
                r.drawFilledCircle(cx2+colW/2, cy2+14, 13, Color(80,140,255,200));
                r.drawFilledCircle(cx2+colW/2, cy2+9, 7, Color(255,255,255,15));
            }
            char buf[8]; snprintf(buf, sizeof(buf), "%d", dateNum);
            if (sf) r.drawTextCentered(sf, buf, cx2, cy2+2, colW, 26,
                isToday ? Color(255,255,255,240) :
                isWknd  ? Color(255,100,100,180) : UITheme::textSecondary());
            // Events
            if (dateNum==19) {
                r.drawRoundedRect(cx2+2, cy2+22, colW-4, 14, 3, Color(80,140,255,180));
                if (sf) r.drawTextCentered(sf, "Team Sync", cx2+2, cy2+22, colW-4, 14,
                    Color(255,255,255,230));
            }
            if (dateNum==22) {
                r.drawRoundedRect(cx2+2, cy2+22, colW-4, 14, 3, Color(70,200,120,180));
                if (sf) r.drawTextCentered(sf, "Release", cx2+2, cy2+22, colW-4, 14,
                    Color(255,255,255,230));
            }
            dateNum++;
        }
    }
    // Today column line
    r.drawLine(cr.x, cr.y+56+28, cr.x, cr.y+cr.h, Color(32,44,80,100));
    for (int i=0; i<7; i++)
        r.drawLine(cr.x+i*colW, cr.y+56+28, cr.x+i*colW, cr.y+cr.h, Color(28,36,65,80));
}

void WindowManager::drawAppContent(Renderer& r, TTF_Font* font, TTF_Font* smallFont,
                                    const Window& win) {
    SDL_Rect cr = win.getContentRect();
    if (cr.w <= 0 || cr.h <= 0) return;

    r.pushClip(cr.x, cr.y, cr.w, cr.h);
    r.drawRect(cr.x, cr.y, cr.w, cr.h, Color(10,13,26,255));

    switch (win.appType) {
    case AppType::TERMINAL:
        s_terminal.render(r, font, smallFont, cr.x, cr.y, cr.w, cr.h); break;
    case AppType::FILES:
        s_files.render(r, font, smallFont, cr.x, cr.y, cr.w, cr.h); break;
    case AppType::SETTINGS:
        s_settings.render(r, font, smallFont, cr.x, cr.y, cr.w, cr.h); break;
    case AppType::TASK_MANAGER:
        s_taskman.render(r, font, smallFont, cr.x, cr.y, cr.w, cr.h); break;
    case AppType::NOTES:
        drawNotesApp(r, font, smallFont, cr); break;
    case AppType::CALENDAR:
        drawCalendarApp(r, font, smallFont, cr); break;
    default:
        if (font) r.drawTextCentered(font, "No content",
            cr.x, cr.y, cr.w, cr.h, UITheme::textMuted());
        break;
    }

    r.popClip();
}

// ─── Main render ──────────────────────────────────────────────────────────────

void WindowManager::render(Renderer& r, TTF_Font* font, TTF_Font* smallFont) {
    for (auto& win : windows) {
        if (win.minimized) continue;
        drawWindowChrome(r, font, smallFont, win);
        drawAppContent(r, font, smallFont, win);
    }
}

const char* WindowManager::getFocusedTitle() const {
    for (int i=(int)windows.size()-1; i>=0; i--) {
        if (windows[i].focused && !windows[i].minimized)
            return windows[i].title.c_str();
    }
    return "";
}
