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
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << t->tm_hour
        << ":" << std::setw(2) << std::setfill('0') << t->tm_min;
    return oss.str();
}

static std::string getDateString() {
    static const char* days[]   = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
    static const char* months[] = {"Jan","Feb","Mar","Apr","May","Jun",
                                   "Jul","Aug","Sep","Oct","Nov","Dec"};
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    std::ostringstream oss;
    oss << days[t->tm_wday] << "  " << months[t->tm_mon] << "  " << t->tm_mday;
    return oss.str();
}

static void drawWifiIcon(Renderer& r, int cx, int cy, int sz, bool connected) {
    Color c = connected ? Color(70,210,245,220) : Color(100,110,150,160);
    for (int i=3; i>=1; i--) {
        int rad = sz * i / 3;
        int alpha = connected ? (60 + i*50) : (30 + i*25);
        // Arc: top semicircle approximation
        for (int dx=-rad; dx<=rad; dx+=2) {
            float t2 = (float)dx/rad;
            int dy = -(int)(rad*sqrtf(1.0f-t2*t2));
            if (dy > 0) continue;
            r.drawLine(cx+dx, cy+dy, cx+dx, cy+dy, Color(c.r,c.g,c.b,(uint8_t)alpha));
        }
    }
    r.drawFilledCircle(cx, cy+2, sz/4+1, c);
}

static void drawBatteryIcon(Renderer& r, int x, int y, int w, int h, float level, bool charging) {
    // Shell
    r.drawRoundedRect(x, y, w-3, h, 2, Color(0,0,0,0));
    for (int b=0; b<2; b++)
        r.drawRoundedRect(x+b, y+b, w-3-b*2, h-b*2, 2-b,
            Color(170,182,215,(uint8_t)(160/(b+1))));
    // Terminal nub
    r.drawRoundedRect(x+w-3, y+h/4, 3, h/2, 1, Color(150,165,210,180));
    // Fill
    int filled = (int)((w-7)*level);
    Color fc = level > 0.5f ? Color(70,200,120,220) :
               level > 0.2f ? Color(255,180,40,220)  : Color(255,70,70,220);
    if (filled > 0)
        r.drawRoundedRect(x+2, y+2, filled, h-4, 1, fc);
    // Inner shine
    if (filled > 4)
        r.drawRoundedRect(x+2, y+2, filled, (h-4)/2, 1, Color(255,255,255,15));
    // Charging bolt
    if (charging) {
        int bx2=x+w/2-2, by2=y+1;
        r.drawLine(bx2+3, by2, bx2, by2+h/2, Color(255,225,50,210));
        r.drawLine(bx2, by2+h/2, bx2+3, by2+h/2, Color(255,225,50,210));
        r.drawLine(bx2+3, by2+h/2, bx2, by2+h-1, Color(255,225,50,210));
    }
}

static void drawVolumeIcon(Renderer& r, int x, int y, int sz, float level) {
    int cy = y+sz/2;
    // Speaker body
    r.drawRect(x, cy-sz/5, sz/4, sz*2/5, Color(175,188,225,215));
    // Cone
    for (int i=0; i<sz/2; i++) {
        int hw = sz/3*i/(sz/2);
        r.drawLine(x+sz/4, cy-hw, x+sz/4+i, cy-hw, Color(0,0,0,0));
        r.drawLine(x+sz/4+i, cy-hw, x+sz/4+i, cy+hw, Color(175,188,225,190));
    }
    // Arcs
    if (level > 0.05f) {
        Color ac(145,160,210,170);
        int ax=x+sz/2+2;
        r.drawLine(ax, cy-sz/4, ax+3, cy-sz/3, ac);
        r.drawLine(ax, cy+sz/4, ax+3, cy+sz/3, ac);
    }
    if (level > 0.4f) {
        Color ac(145,160,210,140);
        int ax=x+sz/2+6;
        r.drawLine(ax, cy-sz/3, ax+3, cy-sz/2+1, ac);
        r.drawLine(ax, cy+sz/3, ax+3, cy+sz/2-1, ac);
    }
}

// Quick settings panel state
static bool s_showQuickPanel = false;
static bool s_darkModeQ      = true;
static bool s_wifiQ          = true;
static bool s_btQ            = true;
static bool s_dndQ           = false;
static bool s_nightQ         = false;
static float s_brightnessQ   = 0.72f;
static float s_volumeQ       = 0.65f;

static void drawQuickToggle(Renderer& r, int x, int y, int w, int h,
                             const char* label, bool on, Color col, TTF_Font* sf) {
    Color bg = on ? Color(col.r/3, col.g/3, col.b/3, 200) : Color(22,28,56,200);
    r.drawRoundedRect(x, y, w, h, 10, bg);
    r.drawRoundedRect(x, y, w, h, 10, on ? Color(col.r,col.g,col.b,80) : Color(45,58,100,60));
    r.drawRoundedRect(x+1, y+1, w-2, h/2, 9, Color(255,255,255,on?12:5));
    // Icon circle
    r.drawFilledCircle(x+h/2, y+h/2, h/2-6, on ? col : Color(70,85,130));
    if (sf) r.drawText(sf, label, x+h-2, y+(h-r.measureTextH(sf,"A"))/2,
        on ? Color(220,232,255,230) : Color(110,125,175,190));
}

void Taskbar::handleEvent(SDL_Event& e, int /*sw*/) {
    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int mx = e.button.x, my = e.button.y;
        // Quick settings toggle — tray area click
        int trayX = 1280 - 240;
        if (mx >= trayX && my >= Y && my < Y+HEIGHT) {
            s_showQuickPanel = !s_showQuickPanel;
        }
        // Panel interactions
        if (s_showQuickPanel) {
            int px=1280-320, py=HEIGHT+4, pw=312, /*ph=370,*/ pad=12;
            int rowH=52, toggleW=(pw-pad*3)/2;
            int ty=py+pad+52+8; // after slider rows
            struct { bool* val; } toggles[] = {
                {&s_wifiQ},{&s_btQ},{&s_darkModeQ},{&s_dndQ},{&s_nightQ}
            };
            int tx2=px+pad;
            for (int i=0; i<5; i++) {
                int col2=i%2, row2=i/2;
                int bx=tx2+col2*(toggleW+pad), by=ty+row2*(rowH+6);
                if (mx>=bx && mx<bx+toggleW && my>=by && my<by+rowH-6)
                    *toggles[i].val = !*toggles[i].val;
            }
            // Brightness slider
            int sliderY=py+pad+8;
            int sliderX=px+48, sliderW=pw-60;
            if (my>=sliderY && my<sliderY+28 && mx>=sliderX && mx<sliderX+sliderW)
                s_brightnessQ=(float)(mx-sliderX)/sliderW;
            // Volume slider
            sliderY+=36;
            if (my>=sliderY && my<sliderY+28 && mx>=sliderX && mx<sliderX+sliderW)
                s_volumeQ=(float)(mx-sliderX)/sliderW;
        }
    }
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        // Close panel if click outside
        if (s_showQuickPanel) {
            int px=1280-320, py=HEIGHT+4, pw=312, ph=380;
            int mx=e.button.x, my=e.button.y;
            if (!(mx>=px && mx<px+pw && my>=py && my<py+ph))
                s_showQuickPanel = false;
        }
    }
}

void Taskbar::render(Renderer& r, TTF_Font* font, TTF_Font* smallFont,
                     const char* activeWindowTitle) {
    const int W = 1280;
    const int H = HEIGHT;

    // ── Background: frosted glass effect ──────────────────────
    r.drawVerticalGradient(0, Y, W, H,
        Color(15,19,40,248), Color(8,12,26,248));
    // Subtle inner top shine
    r.drawLine(0, Y, W, Y, Color(255,255,255,7));
    r.drawLine(0, Y+1, W, Y+1, Color(255,255,255,4));
    // Bottom border with glow
    r.drawLine(0, Y+H-1, W, Y+H-1, Color(50,65,120,130));
    r.drawLine(0, Y+H-2, W, Y+H-2, Color(35,48,85, 70));

    // ── Start / Logo button ───────────────────────────────────
    bool logoHov = (mouseX>=4 && mouseX<=44 && mouseY>=Y+3 && mouseY<=Y+H-3);
    r.drawRoundedRect(4, Y+3, 40, H-6, 10,
        logoHov ? Color(90,120,255,200) : Color(55,80,210,200));
    r.drawRoundedRect(4, Y+3, 40, (H-6)/2, 10, Color(255,255,255,logoHov?22:12));
    r.drawRoundedRect(4, Y+3, 40, H-6, 10, Color(100,135,255,60));
    if (font) r.drawTextCentered(font, "K", 4, Y+3, 40, H-6, Color(255,255,255,248));

    // ── Search bar ────────────────────────────────────────────
    bool srchHov = (mouseX>=52 && mouseX<=280 && mouseY>=Y+5 && mouseY<=Y+H-5);
    int srchAlpha = srchHov ? 210 : 190;
    r.drawRoundedRect(52, Y+5, 228, H-10, 10,
        Color(srchHov?32:22, srchHov?40:30, srchHov?74:60, srchAlpha));
    r.drawRoundedRect(52, Y+5, 228, H-10, 10, Color(52,66,115, srchHov?100:55));
    r.drawRoundedRect(53, Y+6, 226, (H-10)/2, 10, Color(255,255,255,5));
    // Search icon
    r.drawCircleOutline(70, Y+H/2, 6, Color(85,100,155,160));
    r.drawLine(74, Y+H/2+4, 77, Y+H/2+7, Color(85,100,155,160));
    if (smallFont)
        r.drawText(smallFont, "Search ...", 83,
            Y+(H-r.measureTextH(smallFont,"A"))/2,
            Color(srchHov?125:85, srchHov?140:100, srchHov?195:155, 170));

    // ── Active window title ───────────────────────────────────
    if (font && activeWindowTitle && activeWindowTitle[0] != '\0') {
        int titleX = 292;
        r.drawLine(286, Y+8, 286, Y+H-8, Color(45,60,105,100));
        r.drawText(font, activeWindowTitle, titleX,
            Y+(H-r.measureTextH(font,activeWindowTitle))/2,
            UITheme::textSecondary());
    }

    // ── Workspace indicators ──────────────────────────────────
    int wsX = W/2 - 42;
    for (int i=0; i<4; i++) {
        bool active=(i==0);
        int wx=wsX+i*22;
        if (active) {
            r.drawRoundedRect(wx, Y+H/2-5, 16, 10, 3, Color(80,140,255,210));
            r.drawRoundedRect(wx, Y+H/2-5, 16, 5, 3, Color(255,255,255,14));
        } else {
            r.drawRoundedRect(wx, Y+H/2-4, 14, 8, 3, Color(38,50,88,160));
            r.drawRoundedRect(wx, Y+H/2-4, 14, 4, 3, Color(255,255,255,6));
        }
    }

    // ── System tray ───────────────────────────────────────────
    int trayW = 240;
    int trayX = W - trayW - 4;

    r.drawRoundedRect(trayX-4, Y+3, trayW+6, H-6, 8, Color(18,24,50,150));
    r.drawRoundedRect(trayX-4, Y+3, trayW+6, H-6, 8, Color(52,66,115,38));

    int iconY  = Y + H/2;
    int curX   = trayX + 4;

    // Notification bell
    bool notifHov=(mouseX>=curX && mouseX<=curX+22 && mouseY>=Y && mouseY<Y+H);
    r.drawRoundedRect(curX, Y+5, 22, H-10, 6,
        notifHov ? Color(38,50,96,180) : Color(0,0,0,0));
    r.drawFilledCircle(curX+11, iconY-2, 6, Color(155,170,215, notifHov?225:175));
    r.drawRect(curX+7, iconY+3, 8, 4, Color(155,170,215, notifHov?225:175));
    r.drawFilledCircle(curX+11, iconY+8, 3, Color(155,170,215, notifHov?225:175));
    r.drawFilledCircle(curX+17, Y+9, 4, Color(255,75,75,220));
    r.drawFilledCircle(curX+17, Y+9, 3, Color(255,100,100,180));
    curX += 26;

    r.drawLine(curX, Y+8, curX, Y+H-8, Color(46,60,105,100));
    curX += 8;

    // WiFi
    drawWifiIcon(r, curX+7, iconY, 8, s_wifiQ);
    curX += 20;

    // Battery
    drawBatteryIcon(r, curX, iconY-5, 22, 10, 0.78f, false);
    curX += 28;

    // Volume
    drawVolumeIcon(r, curX, iconY-6, 14, s_volumeQ);
    curX += 22;

    r.drawLine(curX, Y+8, curX, Y+H-8, Color(46,60,105,100));
    curX += 8;

    // Clock
    std::string timeStr = getTimeString();
    std::string dateStr = getDateString();
    if (font) {
        int tw = r.measureTextW(font, timeStr);
        r.drawTextShadow(font, timeStr, curX,
            Y+(H/2 - r.measureTextH(font,timeStr))-1,
            UITheme::textPrimary(), Color(0,0,0,55), 1, 1);
        curX += tw + 6;
    }
    if (smallFont) {
        int dw = r.measureTextW(smallFont, dateStr);
        r.drawText(smallFont, dateStr, W-dw-36,
            Y+H - r.measureTextH(smallFont,"A") - 4, UITheme::textMuted());
    }

    // Power button
    bool pwrHov=(mouseX>=W-30 && mouseX<=W-6 && mouseY>=Y+4 && mouseY<Y+H-4);
    r.drawRoundedRect(W-32, Y+4, 24, H-8, 6,
        pwrHov ? Color(200,50,50,200) : Color(32,42,78,175));
    r.drawRoundedRect(W-32, Y+4, 24, (H-8)/2, 6, Color(255,255,255,pwrHov?12:5));
    // Power symbol
    int pc=W-20, py2=Y+H/2;
    r.drawFilledCircle(pc, py2, 6, Color(0,0,0,0));
    r.drawCircleOutline(pc, py2, 6, pwrHov ? Color(255,180,180,220) : Color(150,165,215,190));
    r.drawLine(pc, py2-8, pc, py2-3, pwrHov ? Color(255,180,180,220) : Color(150,165,215,190));
    r.drawLine(pc, py2-7, pc, py2-3, pwrHov ? Color(255,180,180,220) : Color(150,165,215,190));

    // ── Quick settings panel ──────────────────────────────────
    if (s_showQuickPanel) {
        int px=W-322, py=H+4, pw=314, ph=380;

        // Panel shadow
        r.drawRoundedRect(px+6, py+8, pw, ph, 16, Color(0,0,0,90));
        r.drawRoundedRect(px+3, py+4, pw, ph, 16, Color(0,0,0,55));

        // Panel body
        r.drawRoundedVerticalGradient(px, py, pw, ph, 16,
            Color(20,26,52,248), Color(14,20,44,248));
        r.drawRoundedRect(px, py, pw, ph, 16, Color(60,75,140,70));
        r.drawRoundedRect(px+2, py+2, pw-4, ph/4, 14, Color(255,255,255,6));
        // Top arrow
        r.drawLine(px+pw-38, py, px+pw-24, py, Color(20,26,52,248));

        int pad = 12;
        int rowY = py+pad;

        // Brightness
        if (smallFont) r.drawText(smallFont, "Brightness", px+pad, rowY+4, UITheme::textMuted());
        r.drawFilledCircle(px+pad+8, rowY+20, 7, Color(255,220,80,200));
        // Slider
        int slW=pw-60, slX=px+48;
        r.drawRoundedRect(slX, rowY+16, slW, 6, 3, Color(22,28,58,220));
        if (s_brightnessQ>0) r.drawRoundedRect(slX, rowY+16, (int)(slW*s_brightnessQ), 6, 3,
            Color(255,220,80,220));
        r.drawFilledCircle(slX+(int)(slW*s_brightnessQ), rowY+19, 8, Color(255,255,255,245));
        r.drawFilledCircle(slX+(int)(slW*s_brightnessQ), rowY+19, 5, Color(255,220,80,200));
        rowY += 36;

        // Volume
        if (smallFont) r.drawText(smallFont, "Volume", px+pad, rowY+4, UITheme::textMuted());
        drawVolumeIcon(r, px+pad, rowY+10, 16, s_volumeQ);
        r.drawRoundedRect(slX, rowY+16, slW, 6, 3, Color(22,28,58,220));
        if (s_volumeQ>0) r.drawRoundedRect(slX, rowY+16, (int)(slW*s_volumeQ), 6, 3,
            Color(80,140,255,220));
        r.drawFilledCircle(slX+(int)(slW*s_volumeQ), rowY+19, 8, Color(255,255,255,245));
        r.drawFilledCircle(slX+(int)(slW*s_volumeQ), rowY+19, 5, Color(80,140,255,200));
        rowY += 40;

        r.drawLine(px+pad, rowY, px+pw-pad, rowY, Color(40,52,90,120));
        rowY += 8;

        // Toggle buttons (2x3 grid)
        int toggleW=(pw-pad*3)/2;
        int toggleH=46;
        struct { const char* label; bool* val; Color col; } toggles[] = {
            { "Wi-Fi",      &s_wifiQ,   Color( 70,210,245) },
            { "Bluetooth",  &s_btQ,     Color( 70,140,255) },
            { "Dark Mode",  &s_darkModeQ, Color(160,110,255) },
            { "Do Not Dist",&s_dndQ,    Color(255,190,60)  },
            { "Night Light",&s_nightQ,  Color(255,140,50)  },
        };
        for (int i=0; i<5; i++) {
            int col2=i%2, row2=i/2;
            int bx=px+pad+col2*(toggleW+pad), by=rowY+row2*(toggleH+6);
            drawQuickToggle(r, bx, by, toggleW, toggleH,
                toggles[i].label, *toggles[i].val, toggles[i].col, smallFont);
        }
        rowY += 3*(toggleH+6)+6;

        r.drawLine(px+pad, rowY, px+pw-pad, rowY, Color(40,52,90,120));
        rowY += 8;

        // Quick action buttons
        struct { const char* label; Color col; } actions[] = {
            { "Screenshot", Color(80,140,255) },
            { "Record",     Color(255,80,80)  },
            { "Airplane",   Color(70,200,120) },
        };
        int actW=(pw-pad*4)/3;
        for (int i=0; i<3; i++) {
            int bx=px+pad+i*(actW+pad);
            r.drawRoundedRect(bx, rowY, actW, 38, 9,
                Color(actions[i].col.r/4, actions[i].col.g/4, actions[i].col.b/4, 200));
            r.drawRoundedRect(bx, rowY, actW, 38, 9,
                Color(actions[i].col.r, actions[i].col.g, actions[i].col.b, 60));
            r.drawRoundedRect(bx+1, rowY+1, actW-2, 19, 8, Color(255,255,255,8));
            r.drawFilledCircle(bx+actW/2, rowY+14, 8, actions[i].col);
            if (smallFont) r.drawTextCentered(smallFont, actions[i].label,
                bx, rowY+24, actW, 14, UITheme::textMuted());
        }
    }
}
