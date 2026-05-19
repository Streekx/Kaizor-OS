#include "browser_app.hpp"
#include "../gui/ui_theme.hpp"

void BrowserApp::render(Renderer& r, TTF_Font* font, TTF_Font* smallFont,
                         int x, int y, int w, int h) {
    // Background
    r.drawRect(x, y, w, h, Color(12, 16, 30, 255));

    // ─── Navigation bar ───────────────────────────────────────
    int navH = 40;
    r.drawVerticalGradient(x, y, w, navH,
        Color(20, 26, 50, 255), Color(14, 18, 38, 255));
    r.drawLine(x, y + navH, x + w, y + navH, Color(40, 55, 90, 150));

    // Back / Forward / Refresh buttons
    int btnY = y + navH / 2;
    r.drawFilledCircle(x + 16, btnY, 8, Color(35, 44, 80, 220));
    if (font) r.drawTextCentered(font, "<", x + 8, y + 10, 16, 20, UITheme::textSecondary());
    r.drawFilledCircle(x + 38, btnY, 8, Color(35, 44, 80, 220));
    if (font) r.drawTextCentered(font, ">", x + 30, y + 10, 16, 20, UITheme::textSecondary());
    r.drawFilledCircle(x + 60, btnY, 8, Color(35, 44, 80, 220));
    if (font) r.drawTextCentered(font, "↺", x + 52, y + 10, 16, 20, UITheme::textSecondary());

    // Address bar
    int addrX = x + 80, addrW = w - 140;
    r.drawRoundedRect(addrX, y + 6, addrW, 28, 8, Color(24, 30, 58, 240));
    r.drawRoundedOutline(addrX, y + 6, addrW, 28, 8, UITheme::borderSubtle());
    if (smallFont) {
        r.drawText(smallFont, "kaizor://new-tab",
                   addrX + 10, y + 6 + (28 - r.measureTextH(smallFont, "A")) / 2,
                   UITheme::textMuted());
    }

    // Bookmark button
    r.drawFilledCircle(x + w - 24, btnY, 10, Color(35, 44, 80, 220));
    if (font) r.drawTextCentered(font, "★", x + w - 34, y + 10, 20, 20,
                                  UITheme::accentBlue());

    // ─── Bookmarks bar ────────────────────────────────────────
    int bmY = y + navH;
    r.drawRect(x, bmY, w, 28, Color(16, 20, 40, 255));
    r.drawLine(x, bmY + 28, x + w, bmY + 28, Color(35, 48, 80, 100));

    static const char* bookmarks[] = {"Kaizor Docs", "GitHub", "Ports", "FreeBSD.org"};
    int bx = x + 8;
    for (int i = 0; i < 4 && smallFont; i++) {
        int bw = r.measureTextW(smallFont, bookmarks[i]) + 14;
        r.drawRoundedRect(bx, bmY + 4, bw, 20, 5, Color(30, 38, 72, 200));
        r.drawText(smallFont, bookmarks[i], bx + 7, bmY + 6, UITheme::textSecondary());
        bx += bw + 4;
    }

    // ─── New Tab page content ─────────────────────────────────
    int contentY = bmY + 28;
    int contentH = h - navH - 28;

    // Center content: Logo + search
    int centerX = x + w / 2;
    int centerY = contentY + contentH / 3;

    // K logo circle
    r.drawFilledCircle(centerX, centerY - 40, 30, UITheme::accentBlue());
    r.drawFilledCircle(centerX, centerY - 40, 24, Color(10, 14, 30, 255));
    r.drawFilledCircle(centerX, centerY - 40, 16, UITheme::accentPurple());
    if (font) r.drawTextCentered(font, "K", centerX - 8, centerY - 52, 16, 24,
                                  Color(255, 255, 255));

    // Title
    if (font) r.drawTextCentered(font, "Kaizor Browser",
                                  x, centerY - 5, w, 24, UITheme::textPrimary());

    // Search box
    int sboxW = 420, sboxH = 38;
    int sboxX = centerX - sboxW / 2;
    int sboxY = centerY + 24;
    r.drawRoundedRect(sboxX, sboxY, sboxW, sboxH, 10, Color(20, 26, 52, 240));
    r.drawRoundedOutline(sboxX, sboxY, sboxW, sboxH, 10, UITheme::inputBorderFocus());
    if (smallFont) {
        r.drawText(smallFont, "Search the web or enter URL...",
                   sboxX + 14, sboxY + (sboxH - r.measureTextH(smallFont, "A")) / 2,
                   UITheme::textMuted());
    }

    // Quick links
    static const struct { const char* name; Color col; } ql[] = {
        {"Docs",    Color( 80,140,255)},
        {"Mail",    Color(255,100,130)},
        {"Files",   Color( 70,200,120)},
        {"Settings",Color(160,110,255)},
    };
    int qlY = sboxY + sboxH + 24;
    int qlW = 64, qlGap = 16;
    int qlTotalW = 4 * qlW + 3 * qlGap;
    int qlX = centerX - qlTotalW / 2;
    for (int i = 0; i < 4; i++) {
        r.drawRoundedRect(qlX, qlY, qlW, qlW, 14, Color(ql[i].col.r, ql[i].col.g, ql[i].col.b, 40));
        r.drawRoundedOutline(qlX, qlY, qlW, qlW, 14, Color(ql[i].col.r, ql[i].col.g, ql[i].col.b, 80));
        if (smallFont) r.drawTextCentered(smallFont, ql[i].name, qlX, qlY + qlW + 4, qlW, 16,
                                           UITheme::textSecondary());
        qlX += qlW + qlGap;
    }
}
