#include "settings_app.hpp"
#include "../gui/ui_theme.hpp"

static const char* CATEGORIES[] = {
    "General", "Display", "Sound", "Network",
    "Security", "Privacy", "Users", "About"
};
static const Color CAT_COLORS[] = {
    Color( 80,140,255), Color(160,110,255), Color(255, 80,150), Color( 70,200,120),
    Color(255,140, 50), Color( 70,210,245), Color(255,200, 60), Color(200,200,200)
};

struct SettingRow {
    const char* title;
    const char* subtitle;
    bool        toggled;
    bool        isToggle;
};

static const SettingRow ROWS[] = {
    {"Dark Mode",        "Use dark appearance system-wide",   true,  true},
    {"Night Light",      "Reduce blue light after sunset",    false, true},
    {"Animations",       "Enable desktop animations",          true,  true},
    {"Auto-Updates",     "Automatically install updates",      true,  true},
    {"Notifications",    "Show system notifications",          true,  true},
    {"Focus Mode",       "Hide non-essential notifications",   false, true},
    {"Hardware Accel.",  "Use GPU for UI rendering",           true,  true},
};
static const int ROW_COUNT = 7;

void SettingsApp::render(Renderer& r, TTF_Font* font, TTF_Font* smallFont,
                          int x, int y, int w, int h) {
    TTF_Font* sf = smallFont ? smallFont : font;

    r.drawRect(x, y, w, h, Color(12, 16, 30, 255));

    // ─── Sidebar ──────────────────────────────────────────────
    int sideW = 170;
    r.drawRect(x, y, sideW, h, Color(16, 20, 42, 255));
    r.drawLine(x + sideW, y, x + sideW, y + h, Color(35, 48, 80, 150));

    // Search
    r.drawRoundedRect(x + 8, y + 8, sideW - 16, 28, 8, Color(24, 30, 58, 220));
    if (sf) r.drawText(sf, "  Search...", x + 16, y + 8 + (28 - r.measureTextH(sf, "A")) / 2,
                        UITheme::textMuted());

    int catH = 36;
    for (int i = 0; i < 8; i++) {
        int cy = y + 46 + i * catH;
        bool sel = (i == 0);

        if (sel) {
            r.drawRoundedRect(x + 4, cy, sideW - 8, catH - 2, 8,
                              Color(50, 70, 140, 190));
        }

        r.drawFilledCircle(x + 20, cy + catH / 2 - 1, 7, CAT_COLORS[i]);

        if (sf) {
            r.drawText(sf, CATEGORIES[i],
                       x + 34, cy + (catH - r.measureTextH(sf, "A")) / 2,
                       sel ? UITheme::textPrimary() : UITheme::textSecondary());
        }
    }

    // ─── Content area ─────────────────────────────────────────
    int cx = x + sideW + 1;
    int cw = w - sideW - 1;

    // Section header
    if (font) {
        r.drawText(font, "General Settings", cx + 16, y + 16, UITheme::textPrimary());
    }
    r.drawLine(cx + 16, y + 38, cx + cw - 16, y + 38, UITheme::borderSubtle());

    // Setting rows
    int rowH = 60;
    int rowY = y + 50;
    for (int i = 0; i < ROW_COUNT; i++) {
        int ry = rowY + i * rowH;
        if (ry + rowH > y + h - 10) break;

        bool even = (i % 2 == 0);
        if (even) r.drawRect(cx, ry, cw, rowH, Color(16, 20, 40, 80));

        r.drawLine(cx + 8, ry + rowH - 1, cx + cw - 8, ry + rowH - 1,
                   Color(30, 40, 70, 100));

        // Title
        if (font) {
            r.drawText(font, ROWS[i].title, cx + 16, ry + 10, UITheme::textPrimary());
        }
        if (sf) {
            r.drawText(sf, ROWS[i].subtitle, cx + 16, ry + 32, UITheme::textMuted());
        }

        // Toggle switch
        if (ROWS[i].isToggle) {
            int togW = 46, togH = 24;
            int togX = cx + cw - togW - 16;
            int togY = ry + (rowH - togH) / 2;

            Color trackCol = ROWS[i].toggled
                ? Color(60, 140, 255, 230)
                : Color(40, 52, 90, 200);
            r.drawRoundedRect(togX, togY, togW, togH, togH / 2, trackCol);

            int thumbX = ROWS[i].toggled ? togX + togW - togH + 3 : togX + 3;
            r.drawFilledCircle(thumbX + (togH - 6) / 2,
                               togY + togH / 2,
                               togH / 2 - 3,
                               Color(255, 255, 255, 240));
        }
    }

    // ─── Bottom bar ───────────────────────────────────────────
    r.drawRect(x, y + h - 28, w, 28, Color(14, 18, 36, 255));
    r.drawLine(x, y + h - 28, x + w, y + h - 28, UITheme::borderSubtle());
    if (sf) {
        r.drawText(sf, "Kaizor OS  v1.0.0  (FreeBSD 15-CURRENT)",
                   x + sideW + 16, y + h - 20, UITheme::textMuted());
    }
}
