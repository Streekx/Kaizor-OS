#include "settings_app.hpp"
#include "../gui/ui_theme.hpp"
#include <cstring>

static const struct {
    const char* label;
    Color       col;
    const char* icon;
} SIDEBAR_CATS[] = {
    { "Personalization", Color( 80,140,255), "🎨" },
    { "Display",         Color(160,110,255), "🖥" },
    { "Sound",           Color(255, 80,150), "🔊" },
    { "Network",         Color( 70,200,120), "📶" },
    { "Bluetooth",       Color( 70,160,255), "🔵" },
    { "Security",        Color(255,140, 50), "🔒" },
    { "Privacy",         Color(255,200, 60), "🛡" },
    { "Users",           Color( 70,210,245), "👤" },
    { "Accessibility",   Color(200,200,200), "♿" },
    { "About",           Color(140,140,180), "ℹ" },
};
static const int CAT_COUNT = 10;

static const struct {
    const char* title;
    const char* subtitle;
    bool        toggled;
    bool        isToggle;
    bool        hasSlider;
    float       sliderVal;
} SETTING_ROWS[] = {
    { "Dark Mode",          "Use dark appearance system-wide",    true,  true,  false, 0 },
    { "Accent Color",       "Customize your highlight color",     false, false, false, 0 },
    { "Transparency",       "Window blur and glass effects",      true,  true,  false, 0 },
    { "Animations",         "Smooth UI transitions",              true,  true,  false, 0 },
    { "Brightness",         "Screen brightness level",            false, false, true,  0.72f },
    { "Night Light",        "Reduce blue light after sunset",     false, true,  false, 0 },
    { "Auto-Hide Dock",     "Dock hides when not in use",         false, true,  false, 0 },
    { "Font Scale",         "Text size across all apps",          false, false, true,  0.50f },
};
static const int ROW_COUNT = 8;

static const Color ACCENT_COLORS[] = {
    Color( 80,140,255), Color(160,110,255), Color( 70,200,120),
    Color(255,150, 50), Color(255, 80,150), Color( 70,210,245),
};

// Draw a premium toggle switch
static void drawToggle(Renderer& r, int x, int y, int w, int h, bool on, float pulse) {
    Color trackOn (55 + (int)(15*pulse), 130 + (int)(20*pulse), 255, 230);
    Color trackOff(30, 38, 75, 200);

    r.drawRoundedRect(x, y, w, h, h / 2, on ? trackOn : trackOff);

    if (on) {
        r.drawRoundedRect(x + 1, y + 1, w - 2, h - 2, h / 2,
            Color(0, 0, 0, 0));
        r.drawRoundedRect(x, y, w, h / 2, h / 2, Color(255, 255, 255, 15));
    }

    int thumbX = on ? x + w - h + 2 : x + 2;
    int thumbR = h / 2 - 2;
    // Thumb glow
    if (on) {
        r.drawFilledCircle(thumbX + thumbR, y + h / 2, thumbR + 3,
            Color(80, 140, 255, (int)(50 * pulse)));
    }
    r.drawFilledCircle(thumbX + thumbR, y + h / 2, thumbR,
        Color(255, 255, 255, 245));
    // Thumb shine
    r.drawFilledCircle(thumbX + thumbR - 2, y + h / 2 - 2, thumbR / 2,
        Color(255, 255, 255, 80));
}

// Draw a slider
static void drawSlider(Renderer& r, int x, int y, int w, int h, float val) {
    int filled = (int)(w * val);
    // Track
    r.drawRoundedRect(x, y + h / 2 - 3, w, 6, 3, Color(28, 35, 68, 220));
    // Fill
    r.drawRoundedRect(x, y + h / 2 - 3, filled, 6, 3, Color(70, 130, 255, 220));
    // Thumb
    r.drawFilledCircle(x + filled, y + h / 2, 7, Color(255, 255, 255, 245));
    r.drawFilledCircle(x + filled, y + h / 2, 4, Color(70, 130, 255, 220));
}

void SettingsApp::render(Renderer& r, TTF_Font* font, TTF_Font* smallFont,
                          int x, int y, int w, int h) {
    TTF_Font* sf  = smallFont ? smallFont : font;
    TTF_Font* hf  = font;
    const int SIDE_W  = 200;
    const int STATUS_H = 24;
    static float pulseVal = 0.8f;
    pulseVal = 0.7f + 0.3f;

    // ── Background ────────────────────────────────────────────
    r.drawRect(x, y, w, h, Color(10, 14, 28, 255));

    // ── Sidebar ───────────────────────────────────────────────
    r.drawVerticalGradient(x, y, SIDE_W, h,
        Color(15, 20, 40, 255), Color(11, 15, 32, 255));
    r.drawLine(x + SIDE_W, y, x + SIDE_W, y + h, Color(35, 48, 80, 150));

    // Sidebar header / user info
    r.drawVerticalGradient(x, y, SIDE_W, 70,
        Color(20, 28, 58, 255), Color(15, 20, 44, 255));
    r.drawLine(x, y + 70, x + SIDE_W, y + 70, Color(35, 48, 80, 120));

    r.drawFilledCircle(x + 28, y + 35, 20, Color(80, 140, 255, 220));
    r.drawFilledCircle(x + 28, y + 35, 14, Color(15, 22, 48, 255));
    if (hf) r.drawTextCentered(hf, "K", x + 18, y + 24, 20, 22,
        Color(220, 230, 255, 240));

    if (sf) {
        r.drawText(sf, "Kaizor",       x + 52, y + 24, Color(210, 222, 255, 230));
        r.drawText(sf, "Administrator",x + 52, y + 40, UITheme::textMuted());
    }

    // Search
    r.drawRoundedRect(x + 8, y + 78, SIDE_W - 16, 28, 8,
        Color(22, 28, 54, 220));
    r.drawRoundedRect(x + 8, y + 78, SIDE_W - 16, 28, 8,
        Color(50, 65, 110, 80));
    if (sf) r.drawText(sf, "  ⌕  Search settings ...",
        x + 14, y + 78 + (28 - r.measureTextH(sf, "A")) / 2,
        Color(75, 90, 135, 160));

    // Category list
    int catY = y + 116;
    for (int i = 0; i < CAT_COUNT; i++) {
        int iy  = catY + i * 32;
        bool sel = (i == 0);

        if (sel) {
            r.drawRoundedRect(x + 6, iy, SIDE_W - 12, 28, 8,
                Color(45, 62, 135, 200));
            r.drawRect(x + 6, iy, 3, 28, Color(90, 140, 255, 220));
            r.drawRoundedRect(x + 6, iy, SIDE_W - 12, 14, 8,
                Color(255, 255, 255, 6));
        }

        // Icon circle
        r.drawFilledCircle(x + 20, iy + 14, 9, SIDEBAR_CATS[i].col);
        r.drawFilledCircle(x + 20, iy + 14, 5,
            Color(SIDEBAR_CATS[i].col.r / 3,
                  SIDEBAR_CATS[i].col.g / 3,
                  SIDEBAR_CATS[i].col.b / 3, 220));

        if (sf) {
            r.drawText(sf, SIDEBAR_CATS[i].label, x + 36, iy + 8,
                sel ? Color(220, 232, 255, 245) : Color(145, 160, 205, 200));
        }

        if (!sel && sf) {
            r.drawTextRight(sf, "›", x + SIDE_W - 8, iy + 8,
                Color(65, 80, 125, 140));
        }
    }

    // ── Content area ──────────────────────────────────────────
    int cx  = x + SIDE_W + 1;
    int cw  = w - SIDE_W - 1;
    int cy2 = y;
    int ch  = h - STATUS_H;

    r.drawRect(cx, cy2, cw, ch, Color(10, 14, 28, 255));

    // Content header
    int hdrH = 60;
    r.drawVerticalGradient(cx, cy2, cw, hdrH,
        Color(16, 22, 46, 255), Color(12, 16, 36, 255));
    r.drawLine(cx, cy2 + hdrH, cx + cw, cy2 + hdrH, Color(35, 48, 80, 140));

    if (hf) {
        r.drawText(hf, "Personalization", cx + 20, cy2 + 14,
            UITheme::textPrimary());
    }
    if (sf) {
        r.drawText(sf, "Customize the look and feel of your desktop",
            cx + 20, cy2 + 36, UITheme::textMuted());
    }

    // Accent color picker strip (inline in header)
    int acX = cx + cw - 200;
    if (sf) r.drawText(sf, "Accent:", acX - 56, cy2 + 22, UITheme::textMuted());
    for (int i = 0; i < 6; i++) {
        r.drawFilledCircle(acX + i * 28 + 10, cy2 + 30, 9, ACCENT_COLORS[i]);
        if (i == 0) {
            r.drawCircleOutline(acX + i * 28 + 10, cy2 + 30, 11,
                Color(255, 255, 255, 120));
        }
    }

    // Setting rows
    int rowY  = cy2 + hdrH + 8;
    int rowH  = 64;
    int togW  = 50, togH = 26;
    int sldW  = 140;

    for (int i = 0; i < ROW_COUNT; i++) {
        int ry = rowY + i * rowH;
        if (ry + rowH > cy2 + ch - 10) break;

        bool even = (i % 2 == 0);
        if (even) {
            r.drawRect(cx, ry, cw, rowH, Color(14, 18, 38, 80));
        }

        bool isAccent = (i == 1); // Accent color row gets special treatment

        // Left: accent indicator
        r.drawRect(cx, ry + 12, 3, rowH - 24, SIDEBAR_CATS[0].col);

        // Title
        if (hf) {
            r.drawText(hf, SETTING_ROWS[i].title,
                cx + 16, ry + 12, UITheme::textPrimary());
        }
        // Subtitle
        if (sf) {
            r.drawText(sf, SETTING_ROWS[i].subtitle,
                cx + 16, ry + 34, UITheme::textMuted());
        }

        if (isAccent) {
            // Show mini accent swatches
            for (int j = 0; j < 5; j++) {
                r.drawFilledCircle(cx + cw - 160 + j * 26, ry + rowH / 2,
                    7, ACCENT_COLORS[j]);
                if (j == 0) {
                    r.drawCircleOutline(cx + cw - 160 + j * 26, ry + rowH / 2,
                        9, Color(255, 255, 255, 100));
                }
            }
        } else if (SETTING_ROWS[i].isToggle) {
            int tx = cx + cw - togW - 20;
            int ty = ry + (rowH - togH) / 2;
            drawToggle(r, tx, ty, togW, togH, SETTING_ROWS[i].toggled, pulseVal);
        } else if (SETTING_ROWS[i].hasSlider) {
            int sx = cx + cw - sldW - 20;
            int sy2 = ry + (rowH - 20) / 2;
            drawSlider(r, sx, sy2, sldW, 20, SETTING_ROWS[i].sliderVal);
            if (sf) {
                char pct[8];
                snprintf(pct, sizeof(pct), "%d%%",
                    (int)(SETTING_ROWS[i].sliderVal * 100));
                r.drawText(sf, pct, sx + sldW + 8, sy2 + 3,
                    UITheme::textMuted());
            }
        }

        r.drawLine(cx + 10, ry + rowH - 1, cx + cw - 10, ry + rowH - 1,
            Color(28, 36, 65, 120));
    }

    // System info card at bottom
    int cardH = 80;
    int cardY2 = cy2 + ch - cardH - 8;
    if (cardY2 > rowY + ROW_COUNT * rowH + 8) {
        r.drawRoundedVerticalGradient(cx + 16, cardY2, cw - 32, cardH, 12,
            Color(18, 24, 50, 220), Color(13, 18, 40, 220));
        r.drawRoundedRect(cx + 16, cardY2, cw - 32, cardH, 12,
            Color(55, 70, 120, 60));
        r.drawRoundedRect(cx + 16, cardY2, cw - 32, 30, 12,
            Color(255, 255, 255, 5));

        if (hf) {
            r.drawText(hf, "System Information", cx + 28, cardY2 + 10,
                UITheme::textPrimary());
        }
        if (sf) {
            r.drawText(sf, "Kaizor OS  v1.0.0  (FreeBSD 15-CURRENT)  |  Kernel 1.0.0-RELEASE",
                cx + 28, cardY2 + 34, UITheme::textMuted());
            r.drawText(sf, "CPU: Intel Core Ultra 9 285K  |  RAM: 32 GB  |  GPU: KaizorGFX  |  Storage: 2 TB NVMe",
                cx + 28, cardY2 + 52, UITheme::textMuted());
        }
    }

    // ── Status bar ────────────────────────────────────────────
    int stY = y + h - STATUS_H;
    r.drawVerticalGradient(x, stY, w, STATUS_H,
        Color(14, 18, 36, 255), Color(10, 14, 28, 255));
    r.drawLine(x, stY, x + w, stY, Color(35, 48, 80, 130));
    if (sf) {
        r.drawText(sf, "  Personalization  —  8 settings",
            x + 8, stY + (STATUS_H - r.measureTextH(sf, "A")) / 2,
            UITheme::textMuted());
        r.drawTextRight(sf, "All changes saved  ",
            x + w, stY + (STATUS_H - r.measureTextH(sf, "A")) / 2,
            Color(70, 200, 120, 180));
    }
}
