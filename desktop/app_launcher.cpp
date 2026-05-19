#include "app_launcher.hpp"
#include "../gui/ui_theme.hpp"
#include "../gui/taskbar.hpp"
#include <cmath>
#include <algorithm>
#include <cstring>

// ─── Data ─────────────────────────────────────────────────────────────────────

struct AppCategory {
    const char* name;
    Color       iconCol;
};
static const AppCategory CATEGORIES[] = {
    { "Accessories",   Color(200,  55,  55) },
    { "Graphics",      Color( 55, 140, 245) },
    { "Internet",      Color( 50, 175,  55) },
    { "Office",        Color(215, 100,  30) },
    { "Sound & Video", Color(175,  55, 210) },
    { "System Tools",  Color( 90,  90, 100) },
    { "Utilities",     Color( 50, 160, 185) },
    { "Web",           Color( 60, 120, 235) },
};
static const int CAT_COUNT = 8;

struct QuickLoc {
    const char* name;
    Color       col;
};
static const QuickLoc LOCS[] = {
    { "Home",      Color( 80,140,255) },
    { "Desktop",   Color(160,110,255) },
    { "Documents", Color( 70,190,120) },
    { "Downloads", Color(255,150, 50) },
    { "Music",     Color(255, 80,150) },
    { "Pictures",  Color( 70,210,245) },
    { "Videos",    Color(140, 90,255) },
};
static const int LOC_COUNT = 7;

struct SysApp {
    const char* name;
    Color       col;
};
static const SysApp SYS_APPS[] = {
    { "Software",   Color( 50,180,180) },
    { "Settings",   Color(100,100,120) },
    { "Appearance", Color( 90,100,200) },
};
static const int SYS_COUNT = 3;

// ─── Implementation ───────────────────────────────────────────────────────────

AppLauncher::AppLauncher()
    : m_open(false), m_fraction(0.0f), m_mouseX(0), m_mouseY(0)
{}

void AppLauncher::toggle() { m_open ? close() : open(); }
void AppLauncher::open()   { m_open = true; }
void AppLauncher::close()  { m_open = false; }

void AppLauncher::setMousePos(int mx, int my) {
    m_mouseX = mx;
    m_mouseY = my;
}

void AppLauncher::update() {
    const float SPEED = 0.09f;
    if (m_open) {
        m_fraction = std::min(1.0f, m_fraction + SPEED);
    } else {
        m_fraction = std::max(0.0f, m_fraction - SPEED);
    }
}

void AppLauncher::handleEvent(SDL_Event& e) {
    if (!isOpen()) return;

    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
        close();
        return;
    }

    // Close on click outside
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        const int SCRH = 720;
        int panelX = 10;
        int panelY = (int)(SCRH - m_fraction * PANEL_H - Taskbar::HEIGHT - 8);
        int mx = e.button.x, my = e.button.y;
        bool inside = (mx >= panelX && mx < panelX + PANEL_W &&
                       my >= panelY && my < panelY + PANEL_H);
        if (!inside) close();
    }
}

// ── Draw rounded icon square with gradient ────────────────────────────────────
static void drawAppIcon(Renderer& r, int x, int y, int sz, Color col) {
    Color dark(
        std::max(col.r - 50, 0),
        std::max(col.g - 50, 0),
        std::max(col.b - 50, 0), col.a);
    r.drawRoundedVerticalGradient(x, y, sz, sz, sz / 4, col, dark);
    r.drawRoundedRect(x + 1, y + 1, sz - 2, sz / 3, sz / 4,
        Color(255, 255, 255, 22));
    r.drawRoundedRect(x, y, sz, sz, sz / 4,
        Color(255, 255, 255, 35));
}

void AppLauncher::render(Renderer& r, TTF_Font* font, TTF_Font* smallFont) {
    if (m_fraction <= 0.01f) return;

    TTF_Font* sf = smallFont ? smallFont : font;
    TTF_Font* hf = font;

    const int SCRH = 720;

    // Ease the fraction (smooth out)
    float ease = m_fraction * m_fraction * (3.0f - 2.0f * m_fraction);

    // Panel position: slides up from below taskbar
    int panelX = 10;
    int targetY = SCRH - Taskbar::HEIGHT - PANEL_H - 8;
    int startY  = SCRH - Taskbar::HEIGHT + 4;
    int panelY  = startY + (int)((targetY - startY) * ease);

    int alpha = (int)(240 * ease);

    // ── Drop shadow behind the panel ──────────────────────────
    r.drawRoundedRect(panelX + 8,  panelY + 12, PANEL_W, PANEL_H, 16,
        Color(0, 0, 0, (int)(100 * ease)));
    r.drawRoundedRect(panelX + 4,  panelY + 6,  PANEL_W, PANEL_H, 16,
        Color(0, 0, 0, (int)( 60 * ease)));

    // ── Panel glass background ────────────────────────────────
    r.drawRoundedVerticalGradient(panelX, panelY, PANEL_W, PANEL_H, 16,
        Color(20, 26, 52, alpha),
        Color(14, 18, 40, alpha));

    // Glass sheen at top
    r.drawRoundedRect(panelX + 4, panelY + 4, PANEL_W - 8, PANEL_H / 5, 14,
        Color(255, 255, 255, (int)(10 * ease)));

    // Panel border
    for (int brd = 0; brd < 2; brd++) {
        r.drawRoundedRect(panelX + brd, panelY + brd,
            PANEL_W - brd*2, PANEL_H - brd*2, 16 - brd,
            Color(70, 85, 160, (int)(80 * ease) / (brd + 1)));
    }

    // ══════════════════════════════════════════════════════════
    // LEFT COLUMN: App Categories
    // ══════════════════════════════════════════════════════════
    const int LEFT_W  = 265;
    const int DIVIDER = panelX + LEFT_W;
    const int SEARCH_H = 46;
    const int ALLAPP_H = 38;
    const int CAT_H    = 37;
    const int CAT_ICON = 26;

    // Left column bg (slightly darker)
    r.drawRoundedRect(panelX, panelY, LEFT_W, PANEL_H - SEARCH_H, 16,
        Color(0, 0, 0, (int)(30 * ease)));

    // "All Apps" highlighted button at top
    bool allHov = (m_mouseX >= panelX + 8 && m_mouseX < DIVIDER - 8 &&
                   m_mouseY >= panelY + 8  && m_mouseY < panelY + 8 + ALLAPP_H);
    r.drawRoundedRect(panelX + 8, panelY + 8, LEFT_W - 16, ALLAPP_H, 10,
        Color(55, 80, 190, (int)((allHov ? 220 : 170) * ease)));
    r.drawRoundedRect(panelX + 8, panelY + 8, LEFT_W - 16, ALLAPP_H / 2, 10,
        Color(255, 255, 255, (int)(12 * ease)));
    if (sf) {
        r.drawTextCentered(sf, "⊞  All Apps",
            panelX + 8, panelY + 8, LEFT_W - 16, ALLAPP_H,
            Color(220, 230, 255, (int)(230 * ease)));
    }

    // Category label
    if (sf) {
        r.drawText(sf, "CATEGORIES",
            panelX + 14, panelY + ALLAPP_H + 16,
            Color(90, 105, 160, (int)(170 * ease)));
    }

    // App categories
    int catStartY = panelY + ALLAPP_H + 34;
    for (int i = 0; i < CAT_COUNT; i++) {
        int cy = catStartY + i * CAT_H;
        if (cy + CAT_H > panelY + PANEL_H - SEARCH_H - 4) break;

        bool hov = (m_mouseX >= panelX + 4 && m_mouseX < DIVIDER - 4 &&
                    m_mouseY >= cy && m_mouseY < cy + CAT_H);
        if (hov) {
            r.drawRoundedRect(panelX + 4, cy + 2, LEFT_W - 8, CAT_H - 4, 8,
                Color(50, 65, 130, (int)(160 * ease)));
        }

        // App icon
        int iconX = panelX + 12;
        int iconY = cy + (CAT_H - CAT_ICON) / 2;
        drawAppIcon(r, iconX, iconY, CAT_ICON, CATEGORIES[i].iconCol);

        // Name
        if (sf) {
            r.drawText(sf, CATEGORIES[i].name,
                iconX + CAT_ICON + 10, cy + (CAT_H - r.measureTextH(sf, "A")) / 2,
                Color(hov ? 230 : 190, hov ? 238 : 200, 255, (int)(210 * ease)));
        }

        // Arrow
        if (sf) {
            r.drawTextRight(sf, "›",
                DIVIDER - 8, cy + (CAT_H - r.measureTextH(sf, "A")) / 2,
                Color(80, 95, 150, (int)(160 * ease)));
        }
    }

    // ── Vertical divider ─────────────────────────────────────
    r.drawLine(DIVIDER, panelY + 12, DIVIDER, panelY + PANEL_H - SEARCH_H - 12,
        Color(55, 70, 130, (int)(120 * ease)));

    // ══════════════════════════════════════════════════════════
    // RIGHT COLUMN: User + Locations + System + Power
    // ══════════════════════════════════════════════════════════
    int rcX = DIVIDER + 8;
    int rcW = PANEL_W - LEFT_W - 16;
    int ry  = panelY + 12;

    // ── User info section ─────────────────────────────────────
    int avatarR = 22;
    int avatarX = rcX + avatarR + 4;
    int avatarY = ry + avatarR + 6;

    // Avatar circle
    r.drawFilledCircle(avatarX, avatarY, avatarR + 3,
        Color(50, 80, 200, (int)(60 * ease)));
    r.drawFilledCircle(avatarX, avatarY, avatarR,
        Color(60, 90, 200, (int)(220 * ease)));
    r.drawFilledCircle(avatarX, avatarY - 5, avatarR - 6,
        Color(200, 215, 255, (int)(200 * ease)));
    r.drawFilledCircle(avatarX, avatarY + 12, avatarR - 4,
        Color(200, 215, 255, (int)(190 * ease)));

    if (hf) {
        r.drawText(hf, "Kaizor",
            avatarX + avatarR + 10, ry + 12,
            Color(220, 230, 255, (int)(230 * ease)));
    }
    if (sf) {
        r.drawText(sf, "Administrator",
            avatarX + avatarR + 10, ry + 30,
            Color(120, 135, 185, (int)(180 * ease)));
    }

    ry += avatarR * 2 + 18;

    // Separator
    r.drawLine(rcX, ry, rcX + rcW, ry,
        Color(50, 65, 120, (int)(100 * ease)));
    ry += 10;

    // ── Quick locations ───────────────────────────────────────
    if (sf) {
        r.drawText(sf, "PLACES",
            rcX, ry, Color(90, 105, 160, (int)(170 * ease)));
        ry += 18;
    }

    const int LOC_H    = 30;
    const int LOC_ICON = 18;
    for (int i = 0; i < LOC_COUNT; i++) {
        int ly = ry + i * LOC_H;
        if (ly + LOC_H > panelY + PANEL_H - SEARCH_H - 80) break;

        bool hov = (m_mouseX >= rcX && m_mouseX < rcX + rcW &&
                    m_mouseY >= ly  && m_mouseY < ly + LOC_H);
        if (hov) {
            r.drawRoundedRect(rcX - 2, ly + 1, rcW + 4, LOC_H - 2, 7,
                Color(45, 60, 125, (int)(160 * ease)));
        }

        // Folder icon shape
        Color lc = LOCS[i].col;
        Color ld(std::max(lc.r-40,0), std::max(lc.g-40,0), std::max(lc.b-40,0), 255);
        int ix2 = rcX + 4, iy2 = ly + (LOC_H - LOC_ICON) / 2;
        r.drawRoundedVerticalGradient(ix2, iy2 + 3, LOC_ICON, LOC_ICON - 3, 3, lc, ld);
        r.drawRoundedRect(ix2, iy2, LOC_ICON * 2 / 5, 5, 2,
            Color(std::min(lc.r+30,255), std::min(lc.g+30,255), std::min(lc.b+30,255), 255));

        if (sf) {
            r.drawText(sf, LOCS[i].name,
                ix2 + LOC_ICON + 8, ly + (LOC_H - r.measureTextH(sf, "A")) / 2,
                Color(hov ? 230 : 185, hov ? 238 : 198, 255, (int)(210 * ease)));
        }
    }

    ry += LOC_COUNT * LOC_H + 6;

    // Separator
    r.drawLine(rcX, ry, rcX + rcW, ry,
        Color(50, 65, 120, (int)(100 * ease)));
    ry += 8;

    // ── System apps ───────────────────────────────────────────
    for (int i = 0; i < SYS_COUNT; i++) {
        int sy2 = ry + i * LOC_H;
        if (sy2 + LOC_H > panelY + PANEL_H - SEARCH_H - 48) break;

        bool hov = (m_mouseX >= rcX && m_mouseX < rcX + rcW &&
                    m_mouseY >= sy2  && m_mouseY < sy2 + LOC_H);
        if (hov) {
            r.drawRoundedRect(rcX - 2, sy2 + 1, rcW + 4, LOC_H - 2, 7,
                Color(45, 60, 125, (int)(160 * ease)));
        }

        drawAppIcon(r, rcX + 4, sy2 + (LOC_H - 18) / 2, 18, SYS_APPS[i].col);
        if (sf) {
            r.drawText(sf, SYS_APPS[i].name,
                rcX + 30, sy2 + (LOC_H - r.measureTextH(sf, "A")) / 2,
                Color(hov ? 220 : 175, hov ? 228 : 188, 255, (int)(200 * ease)));
        }
    }

    ry += SYS_COUNT * LOC_H + 4;

    // Separator
    r.drawLine(rcX, ry, rcX + rcW, ry,
        Color(50, 65, 120, (int)(100 * ease)));
    ry += 8;

    // ── Power controls ────────────────────────────────────────
    struct { const char* label; Color col; } PWR[] = {
        { "Logout",   Color( 70,130,255) },
        { "Lock",     Color(240,150, 50) },
        { "Shutdown", Color(220,  55,  55) },
    };
    int pwrIconW = (rcW - 8) / 3;
    for (int i = 0; i < 3; i++) {
        int px2 = rcX + i * (pwrIconW + 4);
        int py2 = ry;
        bool hov = (m_mouseX >= px2 && m_mouseX < px2 + pwrIconW &&
                    m_mouseY >= py2  && m_mouseY < py2 + 30);
        Color bg = hov
            ? Color(PWR[i].col.r/2, PWR[i].col.g/2, PWR[i].col.b/2, (int)(200*ease))
            : Color(25, 32, 65, (int)(180*ease));
        r.drawRoundedRect(px2, py2, pwrIconW, 30, 8, bg);
        if (hov) {
            r.drawRoundedRect(px2, py2, pwrIconW, 15, 8,
                Color(255,255,255,(int)(8*ease)));
        }
        r.drawRoundedRect(px2, py2, pwrIconW, 30, 8,
            Color(PWR[i].col.r, PWR[i].col.g, PWR[i].col.b, (int)(60*ease)));
        if (sf) {
            r.drawTextCentered(sf, PWR[i].label, px2, py2, pwrIconW, 30,
                Color(PWR[i].col.r+30 > 255 ? 255 : PWR[i].col.r+30,
                      PWR[i].col.g+30 > 255 ? 255 : PWR[i].col.g+30,
                      PWR[i].col.b+30 > 255 ? 255 : PWR[i].col.b+30,
                      (int)(220*ease)));
        }
    }

    // ══════════════════════════════════════════════════════════
    // SEARCH BAR (full width, at bottom)
    // ══════════════════════════════════════════════════════════
    int sbY = panelY + PANEL_H - SEARCH_H;

    // Search divider
    r.drawLine(panelX + 4, sbY, panelX + PANEL_W - 4, sbY,
        Color(50, 65, 125, (int)(120 * ease)));

    // Search area bg
    r.drawRoundedRect(panelX, sbY, PANEL_W, SEARCH_H, 16,
        Color(12, 16, 35, (int)(200 * ease)));

    // Search box itself
    r.drawRoundedRect(panelX + 10, sbY + 7, PANEL_W - 20, SEARCH_H - 14, 10,
        Color(24, 30, 62, (int)(230 * ease)));
    r.drawRoundedRect(panelX + 10, sbY + 7, PANEL_W - 20, SEARCH_H - 14, 10,
        Color(60, 75, 140, (int)(70 * ease)));

    if (sf) {
        r.drawText(sf, "⌕  Type to search ...",
            panelX + 24, sbY + (SEARCH_H - r.measureTextH(sf, "A")) / 2,
            Color(90, 105, 160, (int)(180 * ease)));
    }
}
