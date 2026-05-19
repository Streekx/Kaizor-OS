#include "taskbar.hpp"
#include "ui_theme.hpp"
#include <ctime>
#include <cstring>
#include <string>

Taskbar::Taskbar() : mouseX(0), mouseY(0) {}

void Taskbar::setMousePos(int mx, int my) {
    mouseX = mx;
    mouseY = my;
}

static std::string getTimeString() {
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    char buf[16];
    snprintf(buf, sizeof(buf), "%02d:%02d", t->tm_hour, t->tm_min);
    return std::string(buf);
}

static std::string getDateString() {
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    static const char* days[]   = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
    static const char* months[] = {"Jan","Feb","Mar","Apr","May","Jun",
                                   "Jul","Aug","Sep","Oct","Nov","Dec"};
    char buf[32];
    snprintf(buf, sizeof(buf), "%s %s %d", days[t->tm_wday], months[t->tm_mon], t->tm_mday);
    return std::string(buf);
}

void Taskbar::render(Renderer& r, TTF_Font* font, TTF_Font* smallFont,
                     const char* activeWindowTitle) {

    const int W = 1280;
    const int H = HEIGHT;

    // ─── Background (gradient glass) ──────────────────────────
    r.drawVerticalGradient(0, Y, W, H,
        Color(18, 22, 42, 238),
        Color(10, 14, 30, 238));

    // Bottom border glow line
    r.drawLine(0, H - 1, W, H - 1, Color(60, 80, 140, 120));
    r.drawLine(0, H - 2, W, H - 2, Color(40, 55, 100, 60));

    // ─── OS Logo / Start button ────────────────────────────────
    bool logoHov = (mouseX >= 6 && mouseX <= 42 && mouseY >= 4 && mouseY <= H - 4);
    Color logoBg = logoHov ? UITheme::accentBlue() : Color(50, 80, 200, 220);
    r.drawRoundedRect(6, 5, 36, H - 10, 8, logoBg);

    // "K" letter in logo
    if (font) {
        r.drawTextCentered(font, "K", 6, 5, 36, H - 10,
                           Color(255, 255, 255, 240));
    }

    // ─── Search / Activity bar ─────────────────────────────────
    r.drawRoundedRect(52, 6, 220, H - 12, 10, UITheme::taskbarSearch());
    r.drawRoundedOutline(52, 6, 220, H - 12, 10, UITheme::borderSubtle());

    if (smallFont) {
        r.drawText(smallFont, "Search apps & files...",
                   68, 6 + (H - 12 - r.measureTextH(smallFont, "A")) / 2,
                   UITheme::textMuted());
    }

    // ─── Active window title ───────────────────────────────────
    if (font && activeWindowTitle && activeWindowTitle[0] != '\0') {
        int titleX = 290;
        r.drawText(font, activeWindowTitle,
                   titleX,
                   (H - r.measureTextH(font, activeWindowTitle)) / 2,
                   UITheme::textSecondary());
    }

    // ─── System tray area ─────────────────────────────────────
    int trayX = W - 8;

    // Clock
    std::string timeStr = getTimeString();
    std::string dateStr = getDateString();

    if (font) {
        int tW = r.measureTextW(font, timeStr);
        r.drawText(font, timeStr,
                   trayX - tW - 68,
                   (H - r.measureTextH(font, timeStr)) / 2,
                   UITheme::textPrimary());
    }
    if (smallFont) {
        int dW = r.measureTextW(smallFont, dateStr);
        r.drawText(smallFont, dateStr,
                   trayX - dW - 68,
                   H - 12,
                   UITheme::textMuted());
    }

    // Tray icons (WiFi, Battery, Volume — drawn as colored blobs)
    int iconY  = H / 2 - 5;
    int icon1x = W - 70;

    // WiFi icon (arc-like, using stacked lines)
    r.drawFilledCircle(icon1x + 5, iconY + 8, 3, UITheme::accentCyan());
    r.drawLine(icon1x,     iconY + 4, icon1x + 10, iconY + 4, UITheme::accentCyan());
    r.drawLine(icon1x + 2, iconY + 1, icon1x + 8,  iconY + 1, Color(70, 180, 220, 160));

    // Battery icon
    int bx = W - 52;
    r.drawOutlineRect(bx, iconY, 16, 10, UITheme::textSecondary());
    r.drawRect(bx + 16, iconY + 3, 2, 4, UITheme::textSecondary());
    r.drawRect(bx + 2,  iconY + 2, 10, 6, UITheme::accentGreen());

    // Volume icon
    int vx = W - 32;
    r.drawRect(vx,      iconY + 2, 4, 6, UITheme::textSecondary());
    r.drawRect(vx + 4,  iconY,     2, 10, UITheme::textSecondary());
    r.drawLine(vx + 7,  iconY + 2, vx + 10, iconY + 2, UITheme::textMuted());
    r.drawLine(vx + 7,  iconY + 8, vx + 10, iconY + 8, UITheme::textMuted());
}
