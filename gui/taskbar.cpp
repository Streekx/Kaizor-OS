#include "taskbar.hpp"
#include "ui_theme.hpp"
#include <ctime>
#include <cstring>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>

Taskbar::Taskbar() : mouseX(0), mouseY(0) {}
void Taskbar::setMousePos(int mx, int my) { mouseX = mx; mouseY = my; }

static std::string getTimeString() {
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    std::ostringstream o;
    o << std::setw(2) << std::setfill('0') << t->tm_hour
      << ":" << std::setw(2) << std::setfill('0') << t->tm_min;
    return o.str();
}
static std::string getDateString() {
    static const char* days[]   = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
    static const char* months[] = {"Jan","Feb","Mar","Apr","May","Jun",
                                   "Jul","Aug","Sep","Oct","Nov","Dec"};
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    std::ostringstream o;
    o << days[t->tm_wday] << "  " << months[t->tm_mon] << "  " << t->tm_mday;
    return o.str();
}

// ── Quick-settings panel state ────────────────────────────────────────────────
static bool  s_showQP     = false;
static bool  s_wifiQ      = true;
static bool  s_btQ        = true;
static bool  s_darkModeQ  = true;
static bool  s_dndQ       = false;
static bool  s_nightQ     = false;
static float s_bright     = 0.72f;
static float s_vol        = 0.65f;

// ── Icon helpers ──────────────────────────────────────────────────────────────
static void drawWifi(Renderer& r, int cx, int cy, int sz, bool on) {
    Color c = on ? Color(70,210,245,220) : Color(100,110,150,160);
    for (int i = 3; i >= 1; i--) {
        int rad = sz * i / 3;
        for (int dx = -rad; dx <= rad; dx += 2) {
            float t2 = (float)dx / rad;
            int   dy2 = -(int)(rad * sqrtf(std::max(0.0f, 1.0f - t2*t2)));
            if (dy2 > 0) continue;
            r.drawLine(cx+dx, cy+dy2, cx+dx, cy+dy2, Color(c.r,c.g,c.b,(uint8_t)(60+i*50)));
        }
    }
    r.drawFilledCircle(cx, cy+2, sz/4+1, c);
}

static void drawBatt(Renderer& r, int x, int y, int w, int h, float lvl) {
    for (int b = 0; b < 2; b++)
        r.drawRoundedRect(x+b, y+b, w-3-b*2, h-b*2, 2-b,
            Color(160,172,215,(uint8_t)(150/(b+1))));
    r.drawRoundedRect(x+w-3, y+h/4, 3, h/2, 1, Color(140,155,200,170));
    int fw = (int)((w-6)*lvl);
    if (fw > 0) {
        Color fc = lvl > 0.5f ? Color(70,200,120,220) :
                   lvl > 0.2f ? Color(255,180,40,220)  : Color(255,70,70,220);
        r.drawRoundedRect(x+2, y+2, fw, h-4, 1, fc);
        r.drawRoundedRect(x+2, y+2, fw, (h-4)/2, 1, Color(255,255,255,15));
    }
    // Charging bolt
    r.drawLine(x+w/2, y+1, x+w/2-2, y+h/2, Color(255,225,50,200));
    r.drawLine(x+w/2-2, y+h/2, x+w/2+1, y+h/2, Color(255,225,50,200));
    r.drawLine(x+w/2+1, y+h/2, x+w/2-1, y+h-1, Color(255,225,50,200));
}

static void drawVol(Renderer& r, int x, int y, int sz, float lvl) {
    int cy = y + sz/2;
    r.drawRect(x, cy-sz/5, sz/4, sz*2/5, Color(175,188,225,215));
    for (int i = 0; i < sz/2; i++) {
        int hw = sz/3*i/(sz/2);
        r.drawLine(x+sz/4+i, cy-hw, x+sz/4+i, cy+hw, Color(175,188,225,180));
    }
    if (lvl > 0.05f) {
        r.drawLine(x+sz/2+2, cy-sz/4,   x+sz/2+5, cy-sz/3,   Color(145,160,210,160));
        r.drawLine(x+sz/2+2, cy+sz/4,   x+sz/2+5, cy+sz/3,   Color(145,160,210,160));
    }
    if (lvl > 0.40f) {
        r.drawLine(x+sz/2+6, cy-sz/3,   x+sz/2+9, cy-sz/2+1, Color(145,160,210,130));
        r.drawLine(x+sz/2+6, cy+sz/3,   x+sz/2+9, cy+sz/2-1, Color(145,160,210,130));
    }
}

static void drawQToggle(Renderer& r, int x, int y, int w, int h,
                         const char* label, bool on, Color col, TTF_Font* sf) {
    Color bg = on ? Color(col.r/4, col.g/4, col.b/4, 200)
                  : Color(22, 28, 56, 200);
    r.drawRoundedRect(x, y, w, h, 10, bg);
    r.drawRoundedRect(x, y, w, h, 10, on ? Color(col.r,col.g,col.b,80) : Color(45,58,100,60));
    r.drawRoundedRect(x+1, y+1, w-2, h/2, 9, Color(255,255,255,on?12:5));
    r.drawFilledCircle(x+h/2, y+h/2, h/2-6, on ? col : Color(70,85,130));
    r.drawFilledCircle(x+h/2, y+h/2 - 4, h/4+1, Color(255,255,255, on?30:12));
    if (sf) r.drawText(sf, label, x+h-2, y+(h-r.measureTextH(sf,"A"))/2,
        on ? Color(220,232,255,230) : Color(110,125,175,190));
}

static void drawToggleSmall(Renderer& r, int x, int y, int w, int h, bool on) {
    Color track = on ? Color(60,130,255,230) : Color(35,44,80,200);
    r.drawRoundedRect(x, y, w, h, h/2, track);
    if (on) r.drawRoundedRect(x+1, y+1, w-2, h/2, h/2, Color(255,255,255,12));
    int tx = on ? x+w-h+2 : x+2;
    r.drawFilledCircle(tx+h/2-2, y+h/2, h/2-2, Color(255,255,255,245));
    r.drawFilledCircle(tx+h/2-4, y+h/2-2, h/4, Color(255,255,255,60));
}

static void drawSlider(Renderer& r, int x, int y, int w, float val, Color col) {
    int filled = (int)(w * val);
    r.drawRoundedRect(x, y+4, w, 6, 3, Color(22,28,58,220));
    r.drawRoundedRect(x, y+4, filled, 6, 3, col);
    r.drawFilledCircle(x+filled, y+7, 8, Color(255,255,255,245));
    r.drawFilledCircle(x+filled, y+7, 5, col);
    r.drawFilledCircle(x+filled-2, y+5, 3, Color(255,255,255,60));
}

// ── handleEvent ───────────────────────────────────────────────────────────────
bool Taskbar::handleEvent(SDL_Event& e, int /*sw*/) {
    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int mx = e.button.x, my = e.button.y;
        const int W = 1280, H = HEIGHT;

        // Notification bell — tray area: approx right zone
        int trayX = W - 240;
        if (mx >= trayX && mx < trayX + 30 && my >= Y && my < Y + H) {
            s_showQP = false;
            return true;   // signal: bell clicked → toggle history
        }

        // Quick settings toggle — rest of tray area
        if (mx >= trayX + 30 && mx < W - 32 && my >= Y && my < Y + H) {
            s_showQP = !s_showQP;
        }

        // Quick panel interactions
        if (s_showQP) {
            int px = W-322, py = H+4, pw = 314;
            int pad = 12;
            int sliderX = px+48, sliderW = pw-60;
            int rowY = py + pad;

            // Brightness
            if (my >= rowY && my < rowY+32 && mx >= sliderX && mx < sliderX+sliderW)
                s_bright = (float)(mx-sliderX)/sliderW;
            rowY += 38;

            // Volume
            if (my >= rowY && my < rowY+32 && mx >= sliderX && mx < sliderX+sliderW)
                s_vol = (float)(mx-sliderX)/sliderW;
            rowY += 42;

            // Toggle buttons
            int togW2 = (pw-pad*3)/2, togH2 = 46;
            struct { bool* val; } togs[] = {&s_wifiQ,&s_btQ,&s_darkModeQ,&s_dndQ,&s_nightQ};
            for (int i = 0; i < 5; i++) {
                int col2 = i%2, row2 = i/2;
                int bx = px+pad+col2*(togW2+pad), by = rowY+row2*(togH2+6);
                if (mx>=bx && mx<bx+togW2 && my>=by && my<by+togH2)
                    *togs[i].val = !*togs[i].val;
            }

            // Close if outside
            if (!(mx >= px && mx < px+pw && my >= py && my < py+400))
                s_showQP = false;
        }
    }
    return false;
}

// ── render ───────────────────────────────────────────────────────────────────
void Taskbar::render(Renderer& r, TTF_Font* font, TTF_Font* smallFont,
                     const char* activeWindowTitle, int unreadCount) {
    TTF_Font* sf = smallFont;
    const int W = 1280;
    const int H = HEIGHT;

    // ── Bar background ────────────────────────────────────────
    r.drawVerticalGradient(0, Y, W, H,
        Color(15,19,40,252), Color(8,12,28,252));
    r.drawLine(0, Y,   W, Y,   Color(255,255,255,5));
    r.drawLine(0, Y+1, W, Y+1, Color(255,255,255,3));
    r.drawLine(0, Y+H-1, W, Y+H-1, Color(55,70,120,140));
    r.drawLine(0, Y+H-2, W, Y+H-2, Color(35,48,85, 70));

    // ── K / Start button ──────────────────────────────────────
    bool kHov = (mouseX>=4 && mouseX<=44 && mouseY>=Y+3 && mouseY<=Y+H-3);
    r.drawRoundedRect(4, Y+3, 40, H-6, 10,
        kHov ? Color(90,120,255,200) : Color(55,80,210,200));
    r.drawRoundedRect(4, Y+3, 40, (H-6)/2, 10, Color(255,255,255,kHov?22:12));
    r.drawRoundedRect(4, Y+3, 40, H-6, 10, Color(100,135,255,60));
    if (font) r.drawTextCentered(font, "K", 4, Y+3, 40, H-6, Color(255,255,255,252));

    // ── Search bar ────────────────────────────────────────────
    bool srchHov = (mouseX>=52 && mouseX<=280 && mouseY>=Y+5 && mouseY<=Y+H-5);
    r.drawRoundedRect(52, Y+5, 228, H-10, 10,
        Color(srchHov?32:22, srchHov?40:30, srchHov?74:60, srchHov?210:190));
    r.drawRoundedRect(52, Y+5, 228, H-10, 10, Color(52,66,115, srchHov?100:55));
    r.drawRoundedRect(53, Y+6, 226, (H-10)/2, 10, Color(255,255,255,5));
    r.drawCircleOutline(70, Y+H/2, 6, Color(85,100,155,160));
    r.drawLine(74, Y+H/2+4, 77, Y+H/2+7, Color(85,100,155,160));
    if (sf) r.drawText(sf, "Search ...", 83,
        Y+(H-r.measureTextH(sf,"A"))/2,
        Color(srchHov?125:85, srchHov?140:100, srchHov?195:155, 170));

    // ── Active window title ───────────────────────────────────
    if (font && activeWindowTitle && activeWindowTitle[0]) {
        r.drawLine(286, Y+8, 286, Y+H-8, Color(45,60,105,100));
        r.drawText(font, activeWindowTitle, 292,
            Y+(H-r.measureTextH(font,activeWindowTitle))/2,
            UITheme::textSecondary());
    }

    // ── Workspace pips ────────────────────────────────────────
    int wsX = W/2 - 42;
    for (int i = 0; i < 4; i++) {
        bool active = (i == 0);
        int wx = wsX + i*22;
        if (active) {
            r.drawRoundedRect(wx, Y+H/2-5, 16, 10, 3, Color(80,140,255,210));
            r.drawRoundedRect(wx, Y+H/2-5, 16, 5,  3, Color(255,255,255,14));
        } else {
            r.drawRoundedRect(wx, Y+H/2-4, 14, 8,  3, Color(38,50,88,160));
        }
    }

    // ── Tray ──────────────────────────────────────────────────
    int trayW  = 250;
    int trayX  = W - trayW - 4;
    r.drawRoundedRect(trayX-4, Y+3, trayW+6, H-6, 8, Color(18,24,50,150));
    r.drawRoundedRect(trayX-4, Y+3, trayW+6, H-6, 8, Color(52,66,115,38));

    int iconY  = Y + H/2;
    int curX   = trayX + 4;

    // ── Notification bell ─────────────────────────────────────
    bool bellHov = (mouseX >= curX && mouseX < curX+26 && mouseY >= Y && mouseY < Y+H);
    r.drawRoundedRect(curX, Y+5, 26, H-10, 6,
        bellHov ? Color(38,50,96,200) : Color(0,0,0,0));

    // Bell body
    r.drawFilledCircle(curX+13, iconY-3, 7,
        Color(155,170,215, bellHov?230:180));
    r.drawRect(curX+7,  iconY+3, 12, 4,
        Color(155,170,215, bellHov?230:180));
    r.drawFilledCircle(curX+13, iconY+8, 3,
        Color(155,170,215, bellHov?230:180));

    // Unread badge
    if (unreadCount > 0) {
        char ub[8]; snprintf(ub, sizeof(ub), "%d", unreadCount > 9 ? 9 : unreadCount);
        int bR = 7;
        r.drawFilledCircle(curX+24, Y+8, bR+1, Color(0,0,0,100));
        r.drawFilledCircle(curX+24, Y+8, bR, Color(255,60,60,235));
        r.drawFilledCircle(curX+22, Y+6, bR/2+1, Color(255,100,100,120));
        if (sf) r.drawTextCentered(sf, ub, curX+24-bR, Y+8-bR, bR*2, bR*2,
            Color(255,255,255,255));
    }
    curX += 30;

    r.drawLine(curX, Y+8, curX, Y+H-8, Color(46,60,105,100));
    curX += 8;

    // WiFi
    drawWifi(r, curX+8, iconY, 8, s_wifiQ);
    curX += 22;

    // Battery
    drawBatt(r, curX, iconY-5, 22, 10, 0.78f);
    curX += 28;

    // Volume
    drawVol(r, curX, iconY-6, 14, s_vol);
    curX += 22;

    r.drawLine(curX, Y+8, curX, Y+H-8, Color(46,60,105,100));
    curX += 8;

    // Clock
    std::string timeStr = getTimeString();
    std::string dateStr = getDateString();
    if (font) {
        int th = r.measureTextH(font, timeStr);
        r.drawTextShadow(font, timeStr, curX,
            Y + H/2 - th - 0,
            UITheme::textPrimary(), Color(0,0,0,55), 1, 1);
        curX += r.measureTextW(font, timeStr) + 6;
    }
    if (sf) {
        r.drawText(sf, dateStr, W - r.measureTextW(sf, dateStr) - 36,
            Y+H - r.measureTextH(sf,"A") - 4, UITheme::textMuted());
    }

    // Power button
    bool pwrHov = (mouseX>=W-32 && mouseX<=W-6 && mouseY>=Y+4 && mouseY<Y+H-4);
    r.drawRoundedRect(W-32, Y+4, 24, H-8, 6,
        pwrHov ? Color(200,50,50,200) : Color(32,42,78,175));
    r.drawRoundedRect(W-32, Y+4, 24, (H-8)/2, 6, Color(255,255,255,pwrHov?12:5));
    int pc = W-20, py2 = Y+H/2;
    r.drawCircleOutline(pc, py2, 6,
        pwrHov ? Color(255,180,180,220) : Color(150,165,215,190));
    r.drawLine(pc, py2-8, pc, py2-3,
        pwrHov ? Color(255,180,180,220) : Color(150,165,215,190));

    // ── Quick settings panel ──────────────────────────────────
    if (!s_showQP) return;

    const int PX = W-322, PY = H+4, PW = 314, PH = 390;

    r.drawRoundedRect(PX+6, PY+8,  PW, PH, 16, Color(0,0,0,90));
    r.drawRoundedRect(PX+3, PY+4,  PW, PH, 16, Color(0,0,0,55));
    r.drawRoundedVerticalGradient(PX, PY, PW, PH, 16,
        Color(20,26,52,250), Color(14,20,44,250));
    r.drawRoundedRect(PX, PY, PW, PH, 16, Color(60,75,140,70));
    r.drawRoundedRect(PX+2, PY+2, PW-4, PH/4, 15, Color(255,255,255,6));

    int pad = 12;
    int rowY = PY + pad;
    int slW  = PW - 60;
    int slX  = PX + 48;

    // Brightness
    if (sf) r.drawText(sf, "Brightness", PX+pad, rowY+2, UITheme::textMuted());
    r.drawFilledCircle(PX+pad+8, rowY+16, 7, Color(255,220,80,200));
    for (int i = 0; i < 8; i++) {
        float a = i * 3.14159f / 4.0f;
        r.drawLine(PX+pad+8+(int)(10*cosf(a)), rowY+16+(int)(10*sinf(a)),
                   PX+pad+8+(int)(13*cosf(a)), rowY+16+(int)(13*sinf(a)),
                   Color(255,220,80,180));
    }
    drawSlider(r, slX, rowY+10, slW, s_bright, Color(255,220,80,220));
    rowY += 38;

    // Volume
    if (sf) r.drawText(sf, "Volume", PX+pad, rowY+2, UITheme::textMuted());
    drawVol(r, PX+pad, rowY+6, 16, s_vol);
    drawSlider(r, slX, rowY+10, slW, s_vol, Color(80,140,255,220));
    rowY += 42;

    r.drawLine(PX+pad, rowY, PX+PW-pad, rowY, Color(40,52,90,120));
    rowY += 8;

    // Toggle grid (2 columns × 3 rows)
    int togW2 = (PW-pad*3)/2, togH2 = 46;
    struct { const char* label; bool* val; Color col; } togs[] = {
        { "Wi-Fi",       &s_wifiQ,    Color( 70,210,245) },
        { "Bluetooth",   &s_btQ,      Color( 70,140,255) },
        { "Dark Mode",   &s_darkModeQ,Color(160,110,255) },
        { "Do Not Dist", &s_dndQ,     Color(255,190, 60) },
        { "Night Light", &s_nightQ,   Color(255,140, 50) },
    };
    for (int i = 0; i < 5; i++) {
        int col2 = i%2, row2 = i/2;
        int bx = PX+pad+col2*(togW2+pad), by = rowY+row2*(togH2+6);
        drawQToggle(r, bx, by, togW2, togH2, togs[i].label, *togs[i].val, togs[i].col, sf);
    }
    rowY += 3*(togH2+6)+6;

    r.drawLine(PX+pad, rowY, PX+PW-pad, rowY, Color(40,52,90,120));
    rowY += 8;

    // Action buttons row
    struct { const char* label; Color col; } acts[] = {
        { "Screenshot", Color(80,140,255) },
        { "Record",     Color(255,80,80)  },
        { "Airplane",   Color(70,200,120) },
    };
    int actW = (PW-pad*4)/3;
    for (int i = 0; i < 3; i++) {
        int bx = PX+pad+i*(actW+pad);
        r.drawRoundedRect(bx, rowY, actW, 38, 9,
            Color(acts[i].col.r/4, acts[i].col.g/4, acts[i].col.b/4, 200));
        r.drawRoundedRect(bx, rowY, actW, 38, 9,
            Color(acts[i].col.r, acts[i].col.g, acts[i].col.b, 60));
        r.drawRoundedRect(bx+1, rowY+1, actW-2, 19, 8, Color(255,255,255,8));
        r.drawFilledCircle(bx+actW/2, rowY+14, 8, acts[i].col);
        r.drawFilledCircle(bx+actW/2, rowY+9,  4, Color(255,255,255,30));
        if (sf) r.drawTextCentered(sf, acts[i].label,
            bx, rowY+24, actW, 14, UITheme::textMuted());
    }
    rowY += 48;

    // DND toggle row at bottom
    r.drawLine(PX+pad, rowY, PX+PW-pad, rowY, Color(40,52,90,120));
    rowY += 6;
    if (sf) r.drawText(sf, "Do Not Disturb", PX+pad+4, rowY+6, UITheme::textSecondary());
    drawToggleSmall(r, PX+PW-60, rowY+4, 46, 22, s_dndQ);
}
