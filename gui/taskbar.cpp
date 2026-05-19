#include "taskbar.hpp"
#include "ui_theme.hpp"
#include <ctime>
#include <cstring>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>

Taskbar::Taskbar() : mouseX(0), mouseY(0) {}

void Taskbar::setMousePos(int mx, int my) {
    mouseX = mx;
    mouseY = my;
}

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

static void drawWifiIcon(Renderer& r, int cx, int cy, int size) {
    int levels = 3;
    for (int i = 0; i < levels; i++) {
        int radius = size * (i + 1) / levels;
        int alpha  = (i < levels - 1) ? 140 : 220;
        Color wc(70, 210, 245, alpha);
        // Draw arc segments as partial circles (top half)
        for (int dy = -radius; dy <= 0; dy++) {
            float t = (float)dy / radius;
            int dx = (int)(radius * sqrtf(1.0f - t * t));
            r.drawLine(cx - dx, cy + dy, cx + dx, cy + dy,
                Color(wc.r, wc.g, wc.b, (uint8_t)(alpha * (float)(dy + radius) / radius * 0.4f)));
        }
    }
    // Dot
    r.drawFilledCircle(cx, cy + 2, size / 4, Color(70, 210, 245, 230));
}

static void drawBatteryIcon(Renderer& r, int x, int y, int w, int h, float level, bool charging) {
    // Outer shell
    r.drawRoundedRect(x,   y,     w - 3, h, 2, Color(0,0,0,0));
    for (int brd = 0; brd < 2; brd++) {
        r.drawRoundedRect(x + brd, y + brd, w - 3 - brd*2, h - brd*2, 2 - brd,
            Color(180, 190, 220, 160 / (brd+1)));
    }
    // Terminal
    r.drawRoundedRect(x + w - 3, y + h/4, 3, h/2, 1, Color(160, 175, 215, 180));

    // Fill
    int filled = (int)((w - 7) * level);
    Color fillCol = level > 0.5f
        ? Color(70, 200, 120, 220)
        : (level > 0.2f ? Color(255, 180, 40, 220) : Color(255, 70, 70, 220));
    if (filled > 0)
        r.drawRoundedRect(x + 2, y + 2, filled, h - 4, 1, fillCol);

    // Charging bolt
    if (charging) {
        int bx = x + w/2 - 2;
        int by = y + 1;
        r.drawLine(bx + 3, by, bx, by + h/2, Color(255, 220, 50, 220));
        r.drawLine(bx, by + h/2, bx + 3, by + h/2, Color(255, 220, 50, 220));
        r.drawLine(bx + 3, by + h/2, bx, by + h - 1, Color(255, 220, 50, 220));
    }
}

static void drawVolumeIcon(Renderer& r, int x, int y, int size, float level) {
    int cy = y + size / 2;
    // Speaker cone
    r.drawRect(x, cy - size/5, size/4, size*2/5, Color(180, 192, 230, 220));
    // Speaker triangle (simplified as filled shape)
    for (int i = 0; i < size/2; i++) {
        int tw2 = (size/3) * i / (size/2);
        r.drawLine(x + size/4, cy - tw2, x + size/4, cy + tw2,
            Color(180, 192, 230, 200));
        r.drawLine(x + size/4 + i, cy - tw2 - 1, x + size/4 + i, cy + tw2 + 1,
            Color(0,0,0,0));
    }
    // Volume arcs
    if (level > 0.0f) {
        int arcX = x + size/2 + 2;
        Color ac(150, 165, 215, 180);
        r.drawLine(arcX, cy - size/4, arcX + 2, cy - size/3,    ac);
        r.drawLine(arcX, cy + size/4, arcX + 2, cy + size/3,    ac);
        if (level > 0.4f) {
            r.drawLine(arcX + 3, cy - size/3, arcX + 5, cy - size/2+1, ac);
            r.drawLine(arcX + 3, cy + size/3, arcX + 5, cy + size/2-1, ac);
        }
    }
}

void Taskbar::render(Renderer& r, TTF_Font* font, TTF_Font* smallFont,
                     const char* activeWindowTitle) {

    const int W = 1280;
    const int H = HEIGHT;

    // ── Background — deep glass gradient ──────────────────────
    r.drawVerticalGradient(0, Y, W, H,
        Color(16, 20, 42, 242),
        Color( 9, 13, 28, 242));

    // Bottom border + inner glow
    r.drawLine(0, Y + H - 1, W, Y + H - 1, Color(55, 70, 130, 130));
    r.drawLine(0, Y + H - 2, W, Y + H - 2, Color(35, 48, 90,  80));

    // Subtle top highlight
    r.drawLine(0, Y,     W, Y,     Color(255,255,255, 6));
    r.drawLine(0, Y + 1, W, Y + 1, Color(255,255,255, 3));

    // ── Start/Logo button ─────────────────────────────────────
    bool logoHov = (mouseX >= 5 && mouseX <= 45 && mouseY >= Y + 3 && mouseY <= Y + H - 3);
    if (logoHov) {
        r.drawRoundedRect(3, Y + 2, 42, H - 4, 10,
            Color(90, 120, 255, 180));
    } else {
        r.drawRoundedRect(3, Y + 2, 42, H - 4, 10,
            Color(55, 80, 210, 200));
    }
    r.drawRoundedRect(4, Y + 3, 40, (H - 4) / 2, 10,
        Color(255, 255, 255, logoHov ? 20 : 12));
    r.drawRoundedRect(3, Y + 2, 42, H - 4, 10,
        Color(100, 130, 255, 60));

    if (font) {
        r.drawTextCentered(font, "K", 3, Y + 2, 42, H - 4,
            Color(255, 255, 255, 245));
    }

    // ── Search bar ────────────────────────────────────────────
    bool srchHov = (mouseX >= 52 && mouseX <= 272 && mouseY >= Y + 5 && mouseY <= Y + H - 5);
    r.drawRoundedRect(52, Y + 5, 220, H - 10, 10,
        Color(srchHov ? 35 : 26, srchHov ? 44 : 34, srchHov ? 80 : 66, 210));
    r.drawRoundedRect(52, Y + 5, 220, H - 10, 10,
        Color(55, 70, 120, srchHov ? 100 : 60));
    r.drawRoundedRect(53, Y + 6, 218, (H - 10) / 2, 10,
        Color(255, 255, 255, 6));

    if (smallFont) {
        r.drawText(smallFont, "⌕  Search ...",
            70, Y + (H - r.measureTextH(smallFont, "A")) / 2,
            Color(srchHov ? 130 : 90, srchHov ? 145 : 105, srchHov ? 195 : 155, 180));
    }

    // ── Active window title ───────────────────────────────────
    if (font && activeWindowTitle && activeWindowTitle[0] != '\0') {
        const int titleX = 286;
        int titleW = r.measureTextW(font, activeWindowTitle);
        // Separator
        r.drawLine(280, Y + 10, 280, Y + H - 10, Color(50, 65, 110, 100));
        r.drawText(font, activeWindowTitle, titleX,
            Y + (H - r.measureTextH(font, activeWindowTitle)) / 2,
            UITheme::textSecondary());

        // Small dot indicator
        r.drawFilledCircle(titleX + titleW + 8, Y + H / 2,
            3, UITheme::accentBlue());
    }

    // ── Workspace switcher ────────────────────────────────────
    int wsX = W / 2 - 40;
    for (int i = 0; i < 4; i++) {
        bool active = (i == 0);
        int wx = wsX + i * 22;
        Color wc = active ? Color(80, 140, 255, 200) : Color(40, 52, 90, 160);
        r.drawRoundedRect(wx, Y + H/2 - 5, 16, 10, 3, wc);
        if (active) {
            r.drawRoundedRect(wx, Y + H/2 - 5, 16, 5, 3,
                Color(255, 255, 255, 12));
        }
    }

    // ── System tray ───────────────────────────────────────────
    int trayPanelW = 220;
    int trayPanelX = W - trayPanelW - 4;

    // Tray panel background
    r.drawRoundedRect(trayPanelX - 4, Y + 3, trayPanelW + 6, H - 6, 8,
        Color(20, 26, 52, 160));
    r.drawRoundedRect(trayPanelX - 4, Y + 3, trayPanelW + 6, H - 6, 8,
        Color(55, 70, 120, 40));

    int iconY  = Y + H / 2;
    int curX   = trayPanelX + 4;

    // Notification bell
    bool notifHov = (mouseX >= curX && mouseX <= curX + 22 && mouseY >= Y && mouseY <= Y + H);
    r.drawRoundedRect(curX, Y + 6, 22, H - 12, 6,
        notifHov ? Color(40, 52, 100, 180) : Color(0,0,0,0));
    // Bell shape
    r.drawFilledCircle(curX + 11, iconY - 2, 6, Color(160, 175, 220, notifHov ? 230 : 180));
    r.drawRect(curX + 7, iconY + 2, 8, 4, Color(160, 175, 220, notifHov ? 230 : 180));
    r.drawFilledCircle(curX + 11, iconY + 7, 3, Color(160, 175, 220, notifHov ? 230 : 180));
    // Notification dot
    r.drawFilledCircle(curX + 16, Y + 10, 4, Color(255, 80, 80, 220));
    curX += 26;

    // Separator
    r.drawLine(curX, Y + 8, curX, Y + H - 8, Color(50, 65, 110, 100));
    curX += 8;

    // WiFi
    drawWifiIcon(r, curX + 6, iconY, 8);
    curX += 20;

    // Battery
    drawBatteryIcon(r, curX, iconY - 5, 22, 10, 0.78f, false);
    curX += 28;

    // Volume
    drawVolumeIcon(r, curX, iconY - 6, 14, 0.65f);
    curX += 22;

    // Separator
    r.drawLine(curX, Y + 8, curX, Y + H - 8, Color(50, 65, 110, 100));
    curX += 8;

    // Clock + Date
    std::string timeStr = getTimeString();
    std::string dateStr = getDateString();

    if (font) {
        int tw = r.measureTextW(font, timeStr);
        r.drawTextShadow(font, timeStr,
            curX, Y + (H / 2 - r.measureTextH(font, timeStr)) - 1,
            UITheme::textPrimary(),
            Color(0, 0, 0, 60), 1, 1);
        curX += tw + 8;
    }

    if (smallFont) {
        int dw = r.measureTextW(smallFont, dateStr);
        r.drawText(smallFont, dateStr,
            W - dw - 8,
            Y + H - r.measureTextH(smallFont, "A") - 4,
            UITheme::textMuted());
    }

    // Power button (far right)
    bool pwrHov = (mouseX >= W - 28 && mouseX <= W - 6 && mouseY >= Y + 4 && mouseY <= Y + H - 4);
    r.drawRoundedRect(W - 30, Y + 4, 24, H - 8, 6,
        pwrHov ? Color(200, 55, 55, 200) : Color(35, 44, 80, 180));
    if (smallFont) {
        r.drawTextCentered(smallFont, "⏻",
            W - 30, Y + 4, 24, H - 8,
            pwrHov ? Color(255, 200, 200, 240) : Color(160, 175, 220, 200));
    }
}
