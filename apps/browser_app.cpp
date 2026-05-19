#include "browser_app.hpp"
#include "../gui/ui_theme.hpp"
#include <cmath>
#include <string>

struct BrowserTab {
    const char* title;
    const char* url;
    bool        active;
    Color       favicon;
};

static BrowserTab TABS[] = {
    { "New Tab",       "kaizor://new-tab",          true,  Color( 80,140,255) },
    { "Kaizor Docs",   "docs.kaizor.io",             false, Color( 70,200,120) },
    { "GitHub",        "github.com",                 false, Color(200,200,200) },
};
static const int TAB_COUNT = 3;

static const struct { const char* name; Color col; const char* icon; } QUICK_LINKS[] = {
    { "Mail",     Color(255,100,130), "✉" },
    { "Docs",     Color( 80,140,255), "📄" },
    { "Files",    Color( 70,200,120), "📁" },
    { "Code",     Color(255,200, 60), "⌨" },
    { "Settings", Color(160,110,255), "⚙" },
    { "Store",    Color( 70,210,245), "⬇" },
    { "News",     Color(255,150, 50), "📰" },
    { "Maps",     Color( 80,200,150), "🗺" },
};
static const int QL_COUNT = 8;

static const struct { const char* site; const char* desc; Color col; } BOOKMARKS[] = {
    { "Kaizor Docs",  "Documentation", Color( 80,140,255) },
    { "FreeBSD.org",  "Operating System", Color(200,100, 50) },
    { "GitHub",       "Source Code",   Color(200,200,200) },
    { "Ports",        "Package Index", Color( 70,200,120) },
};

void BrowserApp::render(Renderer& r, TTF_Font* font, TTF_Font* smallFont,
                         int x, int y, int w, int h) {
    TTF_Font* sf = smallFont;
    const int TAB_H   = 38;
    const int NAV_H   = 42;
    const int BM_H    = 30;

    // ── Background ───────────────────────────────────────────
    r.drawRect(x, y, w, h, Color(11, 15, 28, 255));

    // ── Tab bar ───────────────────────────────────────────────
    r.drawVerticalGradient(x, y, w, TAB_H,
        Color(16, 20, 40, 255), Color(11, 14, 30, 255));

    int tabX = x + 6;
    for (int i = 0; i < TAB_COUNT; i++) {
        bool active = TABS[i].active;
        const char* title = TABS[i].title;
        int tw = sf ? r.measureTextW(sf, title) + 36 : 130;
        tw = std::min(tw, 200);

        if (active) {
            r.drawRoundedRect(tabX, y + 5, tw, TAB_H - 5, 8,
                Color(20, 26, 50, 255));
            r.drawRect(tabX, y + TAB_H - 2, tw, 2,
                Color(80, 140, 255, 220));
        } else {
            bool hov = (i == 2);
            if (hov) {
                r.drawRoundedRect(tabX, y + 7, tw, TAB_H - 7, 7,
                    Color(18, 22, 44, 180));
            }
        }

        // Favicon dot
        r.drawFilledCircle(tabX + 12, y + TAB_H / 2, 5, TABS[i].favicon);

        // Title
        if (sf) {
            r.drawText(sf, title, tabX + 22,
                y + (TAB_H - r.measureTextH(sf, "A")) / 2,
                active ? Color(220, 230, 255, 240) : Color(110, 125, 170, 190));
        }

        // Close button
        r.drawFilledCircle(tabX + tw - 12, y + TAB_H / 2, 5,
            active ? Color(50, 65, 120, 200) : Color(35, 48, 90, 150));
        if (sf) {
            r.drawTextCentered(sf, "×", tabX + tw - 17, y + TAB_H / 2 - 7,
                10, 14, Color(160, 175, 210, 180));
        }

        tabX += tw + 2;
    }

    // New tab button
    int ntW = 28;
    r.drawRoundedRect(tabX + 2, y + 9, ntW, TAB_H - 18, 6,
        Color(30, 40, 75, 200));
    if (sf) r.drawTextCentered(sf, "+", tabX + 2, y + 9, ntW, TAB_H - 18,
        Color(140, 155, 200, 200));

    // ── Navigation bar ────────────────────────────────────────
    int navY = y + TAB_H;
    r.drawVerticalGradient(x, navY, w, NAV_H,
        Color(18, 24, 48, 255), Color(14, 18, 40, 255));
    r.drawLine(x, navY + NAV_H, x + w, navY + NAV_H,
        Color(35, 48, 80, 160));

    // Nav buttons
    struct { const char* sym; bool enabled; } navBtns[] = {
        { "<", false }, { ">", false }, { "↺", true }
    };
    int nbX = x + 8;
    for (auto& btn : navBtns) {
        int nbR = 13;
        int cy2 = navY + NAV_H / 2;
        Color bgCol = btn.enabled
            ? Color(30, 40, 78, 230)
            : Color(22, 28, 55, 160);
        r.drawFilledCircle(nbX + nbR, cy2, nbR, bgCol);
        r.drawCircleOutline(nbX + nbR, cy2, nbR, Color(50, 65, 110, 100));
        if (sf) {
            r.drawTextCentered(sf, btn.sym,
                nbX, cy2 - nbR, nbR * 2, nbR * 2,
                btn.enabled
                    ? Color(190, 205, 245, 220)
                    : Color(70, 85, 130, 160));
        }
        nbX += nbR * 2 + 5;
    }

    // Address bar
    int addrX = nbX + 6;
    int addrW = w - (addrX - x) - 60;
    int addrH = NAV_H - 12;
    r.drawRoundedRect(addrX - 2, navY + 5, addrW + 4, addrH + 4, 12,
        Color(80, 140, 255, 50));
    r.drawRoundedRect(addrX, navY + 7, addrW, addrH, 10,
        Color(20, 26, 54, 240));

    // Lock icon (https)
    r.drawFilledCircle(addrX + 14, navY + NAV_H / 2, 6, Color(70, 200, 120, 200));
    if (sf) {
        r.drawText(sf, "🔒",
            addrX + 8, navY + NAV_H / 2 - 6,
            Color(70, 200, 120, 200));
        r.drawText(sf, " kaizor://new-tab",
            addrX + 24, navY + NAV_H / 2 - r.measureTextH(sf, "A") / 2,
            Color(180, 192, 230, 220));
    }

    // Bookmark star
    int starX = addrX + addrW + 8;
    if (sf) {
        r.drawTextCentered(sf, "☆", starX, navY + 8, 24, NAV_H - 16,
            UITheme::accentBlue());
    }

    // Extension buttons
    r.drawRoundedRect(starX + 26, navY + 8, 28, NAV_H - 16, 7,
        Color(30, 38, 72, 200));
    if (sf) r.drawTextCentered(sf, "⋮", starX + 26, navY + 8, 28, NAV_H - 16,
        Color(150, 165, 210, 200));

    // ── Bookmarks bar ────────────────────────────────────────
    int bmY = navY + NAV_H;
    r.drawRect(x, bmY, w, BM_H, Color(15, 19, 38, 255));
    r.drawLine(x, bmY + BM_H, x + w, bmY + BM_H, Color(30, 42, 72, 120));

    int bx = x + 8;
    for (int i = 0; i < 4; i++) {
        if (!sf) break;
        int bw = r.measureTextW(sf, BOOKMARKS[i].site) + 20;
        r.drawRoundedRect(bx, bmY + 5, bw, BM_H - 10, 6,
            Color(22, 28, 55, 180));
        r.drawFilledCircle(bx + 9, bmY + BM_H / 2, 4, BOOKMARKS[i].col);
        r.drawText(sf, BOOKMARKS[i].site, bx + 17,
            bmY + (BM_H - r.measureTextH(sf, "A")) / 2,
            Color(150, 165, 208, 200));
        bx += bw + 4;
    }

    // ── New Tab Page content ──────────────────────────────────
    int contentY = bmY + BM_H;
    int contentH = h - TAB_H - NAV_H - BM_H;
    int cx       = x + w / 2;

    // Subtle radial background
    r.drawFilledCircle(cx, contentY + contentH / 2, 340,
        Color(25, 12, 60, 18));
    r.drawFilledCircle(cx, contentY + contentH / 2, 220,
        Color(30, 14, 80, 14));

    // Time display
    int centerY = contentY + contentH / 3;
    if (font) {
        r.drawTextCentered(font, "03:21",
            cx - 100, centerY - 50, 200, 40,
            Color(220, 230, 255, 230));
    }
    if (sf) {
        r.drawTextCentered(sf, "Monday,  May 19  2026",
            cx - 180, centerY - 8, 360, 24,
            Color(140, 152, 195, 180));
    }

    // Search box
    int sboxW = 500, sboxH = 42;
    int sboxX = cx - sboxW / 2;
    int sboxY = centerY + 22;
    r.drawRoundedRect(sboxX - 2, sboxY - 2, sboxW + 4, sboxH + 4, 13,
        Color(80, 140, 255, 40));
    r.drawRoundedRect(sboxX, sboxY, sboxW, sboxH, 11,
        Color(20, 26, 52, 240));
    r.drawRoundedRect(sboxX + 1, sboxY + 1, sboxW - 2, sboxH - 2, 10,
        Color(0, 0, 0, 0));
    if (sf) {
        r.drawText(sf, "🔍  Search the web or enter URL ...",
            sboxX + 16, sboxY + (sboxH - r.measureTextH(sf, "A")) / 2,
            Color(85, 100, 148, 180));
    }

    // Quick links grid (2 rows × 4 cols)
    int qlSize   = 60;
    int qlGap    = 14;
    int qlTotal  = QL_COUNT / 2 * (qlSize + qlGap) - qlGap;
    int qlStartX = cx - qlTotal / 2;
    int qlStartY = sboxY + sboxH + 24;

    for (int i = 0; i < QL_COUNT; i++) {
        int col2 = i % 4;
        int row  = i / 4;
        int qx   = qlStartX + col2 * (qlSize + qlGap);
        int qy   = qlStartY + row  * (qlSize + 28 + qlGap);

        Color qc = QUICK_LINKS[i].col;

        // Icon card
        r.drawRoundedRect(qx - 2, qy - 2, qlSize + 4, qlSize + 4, 16,
            Color(qc.r, qc.g, qc.b, 35));
        r.drawRoundedVerticalGradient(qx, qy, qlSize, qlSize, 14,
            Color(qc.r / 4 + 16, qc.g / 4 + 16, qc.b / 4 + 16, 220),
            Color(12, 16, 32, 220));
        r.drawRoundedRect(qx, qy, qlSize, 20, 14,
            Color(255, 255, 255, 8));
        r.drawRoundedOutline(qx, qy, qlSize, qlSize, 14,
            Color(qc.r, qc.g, qc.b, 70), 1);

        // Icon symbol
        if (sf) {
            r.drawTextCentered(sf, QUICK_LINKS[i].icon,
                qx, qy, qlSize, qlSize,
                Color(qc.r + 40 > 255 ? 255 : qc.r + 40,
                      qc.g + 40 > 255 ? 255 : qc.g + 40,
                      qc.b + 40 > 255 ? 255 : qc.b + 40, 220));
        }

        // Label below
        if (sf) {
            r.drawTextCentered(sf, QUICK_LINKS[i].name,
                qx, qy + qlSize + 4, qlSize, 18,
                Color(150, 162, 205, 190));
        }
    }
    (void)font;
}
