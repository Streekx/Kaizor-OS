#include "settings_app.hpp"
#include "../gui/ui_theme.hpp"
#include <cstring>
#include <cstdio>
#include <cmath>

// ─── State ────────────────────────────────────────────────────────────────────
static int   s_cat       = 0;
static int   s_frame     = 0;
static float s_brightness = 0.72f;
static float s_volume     = 0.65f;
static float s_fontScale  = 0.50f;
static bool  s_darkMode   = true;
static bool  s_transparency = true;
static bool  s_animations  = true;
static bool  s_nightLight  = false;
static bool  s_autoHideDock = false;
static bool  s_notifs      = true;
static bool  s_bluetooth   = true;
static bool  s_wifi        = true;
static int   s_resolution  = 1;   // index
static int   s_refreshRate = 0;   // index
static int   s_scaling     = 1;   // index
static int   s_accentColor = 0;

struct SidebarCat {
    const char* label;
    Color       col;
};
static const SidebarCat CATS[] = {
    { "Personalization", Color( 80,140,255) },
    { "Display",         Color(160,110,255) },
    { "Sound",           Color(255, 80,150) },
    { "Network",         Color( 70,200,120) },
    { "Bluetooth",       Color( 70,160,255) },
    { "Security",        Color(255,140, 50) },
    { "Privacy",         Color(255,200, 60) },
    { "Notifications",   Color(255,130, 90) },
    { "Power",           Color( 80,220,160) },
    { "Accessibility",   Color(200,200,200) },
    { "Users",           Color( 70,210,245) },
    { "Storage",         Color(255,190, 60) },
    { "Updates",         Color( 70,200,120) },
    { "About",           Color(140,140,180) },
};
static const int CAT_COUNT = 14;

static const Color ACCENT_COLS[] = {
    Color( 80,140,255), Color(160,110,255), Color( 70,200,120),
    Color(255,150, 50), Color(255, 80,150), Color( 70,210,245),
    Color(255,200, 60), Color(255, 80, 80),
};
static const char* ACCENT_NAMES[] = {
    "Blue","Violet","Green","Orange","Pink","Cyan","Gold","Red"
};

static const char* RESOLUTIONS[] = {
    "1280 x 720  (HD)",
    "1366 x 768  (WXGA)",
    "1600 x 900  (HD+)",
    "1920 x 1080  (Full HD)",
    "2560 x 1440  (QHD)",
    "3840 x 2160  (4K UHD)",
};
static const int RES_COUNT = 6;

static const char* REFRESH_RATES[] = { "60 Hz", "75 Hz", "120 Hz", "144 Hz", "165 Hz", "240 Hz" };
static const int   RF_COUNT = 6;

static const char* SCALING_OPTS[] = { "100%", "125%", "150%", "175%", "200%" };
static const int   SC_COUNT = 5;

// ─── Helpers ──────────────────────────────────────────────────────────────────
SettingsApp::SettingsApp() {}
void SettingsApp::update() { s_frame++; }

static void drawToggle(Renderer& r, int x, int y, int w, int h, bool on) {
    Color track = on ? Color(60, 130, 255, 230) : Color(35, 44, 80, 200);
    r.drawRoundedRect(x, y, w, h, h / 2, track);
    if (on) r.drawRoundedRect(x + 1, y + 1, w - 2, h / 2, h / 2, Color(255,255,255,12));
    int tx = on ? x + w - h + 2 : x + 2;
    r.drawFilledCircle(tx + h/2 - 2, y + h/2, h/2 - 2, Color(255,255,255,245));
    r.drawFilledCircle(tx + h/2 - 4, y + h/2 - 2, h/4, Color(255,255,255,60));
}

static void drawSlider(Renderer& r, int x, int y, int w, int h, float val, Color col) {
    int filled = (int)(w * val);
    r.drawRoundedRect(x, y + h/2 - 3, w, 6, 3, Color(25, 32, 65, 220));
    if (filled > 0) r.drawRoundedRect(x, y + h/2 - 3, filled, 6, 3, col);
    r.drawFilledCircle(x + filled, y + h/2, 8, Color(255,255,255,245));
    r.drawFilledCircle(x + filled, y + h/2, 5, col);
    r.drawFilledCircle(x + filled - 2, y + h/2 - 2, 3, Color(255,255,255,60));
}

static void drawSectionHeader(Renderer& r, TTF_Font* font, int x, int y, int w,
                               const char* title, const char* subtitle) {
    if (font) r.drawText(font, title, x, y, UITheme::textPrimary());
    if (subtitle) {
        // Use a smaller render for subtitle - same font smaller
        r.drawText(font, subtitle, x, y + 28, UITheme::textMuted());
    }
}

static void drawCard(Renderer& r, int x, int y, int w, int h, bool highlight = false) {
    r.drawRoundedVerticalGradient(x, y, w, h, 12,
        Color(20, 26, 52, 235), Color(15, 20, 44, 235));
    r.drawRoundedRect(x, y, w, h, 12,
        highlight ? Color(80,140,255,80) : Color(50, 65, 120, 55));
    r.drawRoundedRect(x + 1, y + 1, w - 2, h / 3, 11, Color(255,255,255,4));
}

static void drawRow(Renderer& r, TTF_Font* hf, TTF_Font* sf,
                    int x, int y, int w, int h,
                    const char* title, const char* sub, bool even) {
    if (even) r.drawRect(x, y, w, h, Color(14, 18, 40, 60));
    r.drawRect(x, y + 10, 3, h - 20, CATS[s_cat].col);
    if (hf) r.drawText(hf, title, x + 14, y + 10, UITheme::textPrimary());
    if (sf && sub) r.drawText(sf, sub, x + 14, y + 30, UITheme::textMuted());
    r.drawLine(x + 8, y + h - 1, x + w - 8, y + h - 1, Color(28, 36, 65, 100));
}

void SettingsApp::handleEvent(SDL_Event& e, int x, int y, int w, int h) {
    const int SIDE_W  = 210;
    const int STATUS_H = 26;
    const int HDR_H   = 70;
    const int ROW_H   = 62;
    const int CAT_H   = 34;
    const int catStartY = y + 120;
    (void)h;

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int mx = e.button.x, my = e.button.y;

        // Sidebar category click
        for (int i = 0; i < CAT_COUNT; i++) {
            int iy = catStartY + i * CAT_H;
            if (mx >= x + 4 && mx < x + SIDE_W - 4 && my >= iy && my < iy + CAT_H) {
                s_cat = i;
                return;
            }
        }

        int cx = x + SIDE_W + 1;
        int cw = w - SIDE_W - 1;
        int rowY = y + HDR_H + 12;

        // Personalization
        if (s_cat == 0) {
            // Dark mode row
            int togW = 52, togH = 28;
            int ry = rowY;
            if (mx >= cx + cw - togW - 20 && mx < cx + cw - 20 && my >= ry && my < ry + ROW_H)
                s_darkMode = !s_darkMode;
            // Transparency
            ry += ROW_H;
            if (mx >= cx + cw - togW - 20 && mx < cx + cw - 20 && my >= ry && my < ry + ROW_H)
                s_transparency = !s_transparency;
            // Animations
            ry += ROW_H;
            if (mx >= cx + cw - togW - 20 && mx < cx + cw - 20 && my >= ry && my < ry + ROW_H)
                s_animations = !s_animations;
            // Auto-hide dock
            ry += ROW_H;
            if (mx >= cx + cw - togW - 20 && mx < cx + cw - 20 && my >= ry && my < ry + ROW_H)
                s_autoHideDock = !s_autoHideDock;
            // Accent color
            ry += ROW_H;
            for (int i = 0; i < 8; i++) {
                int ax = cx + 14 + i * 34;
                int ay = ry + ROW_H / 2 - 10;
                int dx = mx - (ax + 10), dy = my - (ay + 10);
                if (dx * dx + dy * dy <= 10 * 10) {
                    s_accentColor = i;
                    break;
                }
            }
        }

        // Display
        if (s_cat == 1) {
            // Brightness slider area: row 0
            int sliderX = cx + cw - 160 - 20;
            int ry = rowY;
            if (my >= ry && my < ry + ROW_H && mx >= sliderX && mx < sliderX + 160)
                s_brightness = (float)(mx - sliderX) / 160.0f;
            // Night light toggle: row 1
            ry += ROW_H;
            int togW = 52;
            if (mx >= cx + cw - togW - 20 && mx < cx + cw - 20 && my >= ry && my < ry + ROW_H)
                s_nightLight = !s_nightLight;
            // Resolution: row 2 onwards
            ry += ROW_H + 20;
            for (int i = 0; i < RES_COUNT; i++) {
                int iry = ry + i * 42;
                if (mx >= cx + 14 && mx < cx + cw - 14 && my >= iry && my < iry + 38)
                    s_resolution = i;
            }
        }

        // Sound
        if (s_cat == 2) {
            int sliderX = cx + cw - 180 - 20;
            int ry = rowY;
            if (my >= ry && my < ry + ROW_H && mx >= sliderX && mx < sliderX + 180)
                s_volume = (float)(mx - sliderX) / 180.0f;
        }

        // Notifications
        if (s_cat == 7) {
            int togW = 52;
            int ry = rowY;
            if (mx >= cx + cw - togW - 20 && mx < cx + cw - 20 && my >= ry && my < ry + ROW_H)
                s_notifs = !s_notifs;
        }

        // Bluetooth
        if (s_cat == 4) {
            int togW = 52;
            int ry = rowY;
            if (mx >= cx + cw - togW - 20 && mx < cx + cw - 20 && my >= ry && my < ry + ROW_H)
                s_bluetooth = !s_bluetooth;
        }

        // Network
        if (s_cat == 3) {
            int togW = 52;
            int ry = rowY;
            if (mx >= cx + cw - togW - 20 && mx < cx + cw - 20 && my >= ry && my < ry + ROW_H)
                s_wifi = !s_wifi;
        }
    }
}

void SettingsApp::render(Renderer& r, TTF_Font* font, TTF_Font* smallFont,
                          int x, int y, int w, int h) {
    TTF_Font* sf  = smallFont ? smallFont : font;
    TTF_Font* hf  = font;
    const int SIDE_W   = 210;
    const int STATUS_H = 26;
    const int HDR_H    = 70;
    const int ROW_H    = 62;
    const int CAT_H    = 34;
    const float pulse  = 0.65f + 0.35f * sinf(s_frame * 0.04f);

    // ── Background ────────────────────────────────────────────
    r.drawRect(x, y, w, h, Color(9, 12, 26, 255));

    // ── Sidebar ───────────────────────────────────────────────
    r.drawVerticalGradient(x, y, SIDE_W, h,
        Color(13, 17, 36, 255), Color(10, 13, 30, 255));
    r.drawLine(x + SIDE_W, y, x + SIDE_W, y + h, Color(32, 44, 78, 150));

    // User profile area
    r.drawVerticalGradient(x, y, SIDE_W, 80,
        Color(18, 24, 52, 255), Color(13, 18, 44, 255));
    r.drawLine(x, y + 80, x + SIDE_W, y + 80, Color(32, 44, 78, 120));

    int aR = 22;
    r.drawFilledCircle(x + aR + 14, y + 40, aR + 4,
        Color(CATS[s_cat].col.r, CATS[s_cat].col.g, CATS[s_cat].col.b,
              (uint8_t)(50 * pulse)));
    r.drawFilledCircle(x + aR + 14, y + 40, aR, Color(50, 70, 150, 220));
    r.drawFilledCircle(x + aR + 14, y + 32, aR - 7, Color(190, 205, 240, 200));
    r.drawFilledCircle(x + aR + 14, y + 52, aR - 4, Color(190, 205, 240, 180));

    if (sf) {
        r.drawText(sf, "Kaizor",        x + aR * 2 + 22, y + 24, Color(210, 225, 255, 230));
        r.drawText(sf, "Administrator", x + aR * 2 + 22, y + 40, UITheme::textMuted());
    }

    // Search bar
    r.drawRoundedRect(x + 8, y + 88, SIDE_W - 16, 26, 8, Color(20, 26, 52, 220));
    r.drawRoundedRect(x + 8, y + 88, SIDE_W - 16, 26, 8, Color(50, 65, 110, 70));
    if (sf) r.drawText(sf, "  Search ...", x + 18,
        y + 88 + (26 - r.measureTextH(sf, "A")) / 2, Color(70, 85, 130, 150));

    // Categories
    int catStartY = y + 122;
    for (int i = 0; i < CAT_COUNT; i++) {
        int iy = catStartY + i * CAT_H;
        if (iy + CAT_H > y + h - 4) break;
        bool sel = (i == s_cat);

        if (sel) {
            r.drawRoundedRect(x + 4, iy + 2, SIDE_W - 8, CAT_H - 4, 8,
                Color(40, 56, 120, 200));
            r.drawRect(x + 4, iy + 2, 3, CAT_H - 4, CATS[i].col);
            r.drawRoundedRect(x + 4, iy + 2, SIDE_W - 8, (CAT_H - 4) / 2, 8,
                Color(255,255,255,5));
        }

        r.drawFilledCircle(x + 22, iy + CAT_H / 2, 9, CATS[i].col);
        r.drawFilledCircle(x + 22, iy + CAT_H / 2, 5,
            Color(CATS[i].col.r/3, CATS[i].col.g/3, CATS[i].col.b/3, 220));

        if (sf) r.drawText(sf, CATS[i].label, x + 36, iy + (CAT_H - r.measureTextH(sf, "A")) / 2,
            sel ? Color(220, 232, 255, 245) : Color(145, 160, 205, 200));

        if (!sel && sf) r.drawTextRight(sf, ">",
            x + SIDE_W - 8, iy + (CAT_H - r.measureTextH(sf, "A")) / 2,
            Color(60, 75, 120, 130));
    }

    // ── Content area ──────────────────────────────────────────
    int cx  = x + SIDE_W + 1;
    int cw  = w - SIDE_W - 1;

    r.drawRect(cx, y, cw, h, Color(9, 12, 26, 255));

    // Content header
    r.drawVerticalGradient(cx, y, cw, HDR_H,
        Color(15, 20, 44, 255), Color(11, 15, 35, 255));
    r.drawLine(cx, y + HDR_H, cx + cw, y + HDR_H, Color(32, 44, 78, 140));

    // Category icon in header
    r.drawFilledCircle(cx + 28, y + HDR_H / 2, 18,
        Color(CATS[s_cat].col.r, CATS[s_cat].col.g, CATS[s_cat].col.b, 40));
    r.drawFilledCircle(cx + 28, y + HDR_H / 2, 14, CATS[s_cat].col);
    r.drawFilledCircle(cx + 28, y + HDR_H / 2, 9,
        Color(CATS[s_cat].col.r/3, CATS[s_cat].col.g/3, CATS[s_cat].col.b/3, 220));

    if (hf) r.drawText(hf, CATS[s_cat].label, cx + 54, y + 14, UITheme::textPrimary());
    if (sf) {
        static const char* subtitles[] = {
            "Customize the look and feel of your desktop",
            "Configure display resolution, brightness and scaling",
            "Manage audio output, input and notification sounds",
            "Wi-Fi, Ethernet, VPN and network proxy settings",
            "Bluetooth devices, pairing and visibility",
            "Passwords, screen lock and system security",
            "App permissions, location, camera and microphone access",
            "App and system notification preferences",
            "Battery life, sleep, and performance modes",
            "Font size, contrast, screen reader and input aids",
            "User accounts, profiles and permissions",
            "Disk usage, storage cleanup and external drives",
            "System updates, automatic update scheduling",
            "Kaizor OS version, hardware info and licenses",
        };
        r.drawText(sf, subtitles[s_cat], cx + 54, y + 36, UITheme::textMuted());
    }

    int rowY = y + HDR_H + 12;
    const int togW = 52, togH = 28;
    const int sldW = 160;

    // ═══════════════════════════════════════════════════════════
    // PERSONALIZATION
    // ═══════════════════════════════════════════════════════════
    if (s_cat == 0) {
        struct { const char* title; const char* sub; bool* tog; } rows[] = {
            { "Dark Mode",       "Use dark appearance across all windows and apps", &s_darkMode      },
            { "Transparency",    "Enable glass blur effects on windows and panels",  &s_transparency  },
            { "Smooth Animations","Enable transition and micro-animations",          &s_animations    },
            { "Auto-hide Dock",  "Dock hides automatically when not in use",        &s_autoHideDock  },
        };
        for (int i = 0; i < 4; i++) {
            int ry = rowY + i * ROW_H;
            drawRow(r, hf, sf, cx, ry, cw, ROW_H, rows[i].title, rows[i].sub, i % 2 == 0);
            drawToggle(r, cx + cw - togW - 20, ry + (ROW_H - togH) / 2, togW, togH, *rows[i].tog);
        }

        // Accent color row
        int ry = rowY + 4 * ROW_H;
        drawRow(r, hf, sf, cx, ry, cw, ROW_H + 16, "Accent Color",
            "Choose a highlight color for buttons and selections", 0 % 2 == 0);
        for (int i = 0; i < 8; i++) {
            int ax = cx + 14 + i * 34;
            int ay = ry + 10;
            r.drawFilledCircle(ax + 10, ay + 10, 11,
                Color(ACCENT_COLS[i].r, ACCENT_COLS[i].g, ACCENT_COLS[i].b, 60));
            r.drawFilledCircle(ax + 10, ay + 10, 9, ACCENT_COLS[i]);
            if (i == s_accentColor) {
                r.drawCircleOutline(ax + 10, ay + 10, 12, Color(255,255,255,180));
                r.drawFilledCircle(ax + 10, ay + 10, 4, Color(255,255,255,220));
            }
            if (sf) r.drawTextCentered(sf, ACCENT_NAMES[i], ax - 6, ay + 22, 32, 12,
                UITheme::textMuted());
        }

        // Font scale slider row
        ry += ROW_H + 16;
        drawRow(r, hf, sf, cx, ry, cw, ROW_H, "Font Scale",
            "Adjust text size across all apps and system UI", 1 % 2 == 0);
        drawSlider(r, cx + cw - sldW - 20, ry + (ROW_H - 20) / 2, sldW, 20,
            s_fontScale, CATS[s_cat].col);
        char pct[8]; snprintf(pct, sizeof(pct), "%d%%", 75 + (int)(s_fontScale * 50));
        if (sf) r.drawText(sf, pct, cx + cw - 20 + 12, ry + (ROW_H - 20) / 2 + 4,
            UITheme::textMuted());

        // Wallpaper card
        ry += ROW_H + 8;
        drawCard(r, cx + 12, ry, cw - 24, 64);
        r.drawRoundedRect(cx + 12, ry, cw - 24, 64, 12, Color(0,0,0,0));
        if (hf) r.drawText(hf, "Wallpaper", cx + 28, ry + 10, UITheme::textPrimary());
        if (sf) r.drawText(sf, "Current: Kaizor default  |  Click to change",
            cx + 28, ry + 32, UITheme::textMuted());
        // wallpaper mini preview
        r.drawRoundedVerticalGradient(cx + cw - 120, ry + 8, 96, 48, 8,
            Color(30, 80, 180, 255), Color(100, 40, 160, 255));
        r.drawFilledCircle(cx + cw - 72, ry + 32, 20, Color(180, 210, 255, 60));
        r.drawRoundedRect(cx + cw - 120, ry + 8, 96, 48, 8, Color(255,255,255,20));
    }

    // ═══════════════════════════════════════════════════════════
    // DISPLAY
    // ═══════════════════════════════════════════════════════════
    if (s_cat == 1) {
        // Brightness
        drawRow(r, hf, sf, cx, rowY, cw, ROW_H, "Brightness",
            "Adjust screen brightness level", true);
        drawSlider(r, cx + cw - 180 - 20, rowY + (ROW_H - 20) / 2, 180, 20,
            s_brightness, CATS[s_cat].col);
        char pct[8]; snprintf(pct, sizeof(pct), "%.0f%%", s_brightness * 100.0f);
        if (sf) r.drawText(sf, pct, cx + cw - 20 + 12, rowY + (ROW_H - 20) / 2 + 4,
            UITheme::textMuted());

        // Night light
        drawRow(r, hf, sf, cx, rowY + ROW_H, cw, ROW_H, "Night Light",
            "Reduce blue light after sunset to help sleep", false);
        drawToggle(r, cx + cw - togW - 20, rowY + ROW_H + (ROW_H - togH) / 2, togW, togH, s_nightLight);

        // Resolution heading
        int secY = rowY + ROW_H * 2 + 20;
        if (hf) r.drawText(hf, "Resolution", cx + 14, secY, UITheme::textPrimary());
        r.drawLine(cx + 14, secY + 24, cx + cw - 14, secY + 24, Color(35, 48, 80, 120));
        secY += 32;

        for (int i = 0; i < RES_COUNT; i++) {
            int ry = secY + i * 42;
            bool sel = (i == s_resolution);
            r.drawRoundedRect(cx + 14, ry, cw - 28, 38, 8,
                sel ? Color(40, 60, 140, 220) : Color(16, 20, 44, 200));
            r.drawRoundedRect(cx + 14, ry, cw - 28, 38, 8,
                sel ? Color(80,140,255,80) : Color(45, 60, 110, 50));
            if (sel) {
                r.drawRect(cx + 14, ry, 3, 38, CATS[s_cat].col);
                r.drawRoundedRect(cx + 14, ry, cw - 28, 19, 8, Color(255,255,255,4));
            }
            if (sf) r.drawText(sf, RESOLUTIONS[i], cx + 24, ry + (38 - r.measureTextH(sf, "A")) / 2,
                sel ? UITheme::textPrimary() : UITheme::textSecondary());
            if (sel && sf) r.drawTextRight(sf, "Active", cx + cw - 18,
                ry + (38 - r.measureTextH(sf, "A")) / 2, Color(80,200,120,220));
        }

        // Refresh rate + Scaling
        int botY = secY + RES_COUNT * 42 + 12;
        if (botY + 100 < y + h - STATUS_H) {
            // Refresh rate
            if (hf) r.drawText(hf, "Refresh Rate", cx + 14, botY, UITheme::textPrimary());
            r.drawLine(cx + 14, botY + 24, cx + cw - 14, botY + 24, Color(35,48,80,120));
            botY += 32;
            int rfW = (cw - 28 - 10 * (RF_COUNT - 1)) / RF_COUNT;
            for (int i = 0; i < RF_COUNT; i++) {
                int rx = cx + 14 + i * (rfW + 10);
                bool sel = (i == s_refreshRate);
                r.drawRoundedRect(rx, botY, rfW, 36, 8,
                    sel ? Color(40,60,140,220) : Color(16,20,44,200));
                r.drawRoundedRect(rx, botY, rfW, 36, 8,
                    sel ? Color(80,140,255,80) : Color(45,60,110,50));
                if (sel) r.drawRoundedRect(rx, botY, rfW, 18, 8, Color(255,255,255,4));
                if (sf) r.drawTextCentered(sf, REFRESH_RATES[i], rx, botY, rfW, 36,
                    sel ? UITheme::textPrimary() : UITheme::textMuted());
            }
        }
    }

    // ═══════════════════════════════════════════════════════════
    // SOUND
    // ═══════════════════════════════════════════════════════════
    if (s_cat == 2) {
        drawRow(r, hf, sf, cx, rowY, cw, ROW_H, "Output Volume",
            "Main system audio volume", true);
        drawSlider(r, cx + cw - 180 - 20, rowY + (ROW_H - 20) / 2, 180, 20,
            s_volume, CATS[s_cat].col);
        char pct[8]; snprintf(pct, sizeof(pct), "%.0f%%", s_volume * 100.0f);
        if (sf) r.drawText(sf, pct, cx + cw - 20 + 12, rowY + (ROW_H - 20) / 2 + 4,
            UITheme::textMuted());

        struct { const char* dev; const char* type; bool active; } devices[] = {
            { "Built-in Speakers",    "Internal",       true  },
            { "HDMI Audio",           "HDMI / Display", false },
            { "USB Headphones",       "USB Audio",      false },
            { "Bluetooth Headset",    "Bluetooth",      false },
        };
        int devY = rowY + ROW_H + 20;
        if (hf) r.drawText(hf, "Output Devices", cx + 14, devY, UITheme::textPrimary());
        r.drawLine(cx + 14, devY + 24, cx + cw - 14, devY + 24, Color(35,48,80,120));
        devY += 32;
        for (int i = 0; i < 4; i++) {
            int ry = devY + i * 52;
            drawCard(r, cx + 12, ry, cw - 24, 44, devices[i].active);
            r.drawFilledCircle(cx + 30, ry + 22, 10,
                devices[i].active ? CATS[s_cat].col : Color(50, 60, 100));
            if (sf) r.drawText(sf, devices[i].dev, cx + 46, ry + 8, UITheme::textPrimary());
            if (sf) r.drawText(sf, devices[i].type, cx + 46, ry + 24, UITheme::textMuted());
            if (devices[i].active && sf)
                r.drawTextRight(sf, "Active", cx + cw - 18, ry + 8, Color(70,200,120,220));
        }
    }

    // ═══════════════════════════════════════════════════════════
    // NETWORK
    // ═══════════════════════════════════════════════════════════
    if (s_cat == 3) {
        drawRow(r, hf, sf, cx, rowY, cw, ROW_H, "Wi-Fi",
            "Wireless network connection", true);
        drawToggle(r, cx + cw - togW - 20, rowY + (ROW_H - togH) / 2, togW, togH, s_wifi);

        if (s_wifi) {
            int netY = rowY + ROW_H + 20;
            if (hf) r.drawText(hf, "Available Networks", cx + 14, netY, UITheme::textPrimary());
            r.drawLine(cx + 14, netY + 24, cx + cw - 14, netY + 24, Color(35,48,80,120));
            netY += 32;

            struct { const char* ssid; int bars; bool secured; bool connected; } nets[] = {
                { "KaizorNet_5G",    4, true,  true  },
                { "HomeNetwork",     3, true,  false },
                { "Office_WiFi",     2, true,  false },
                { "Guest_Network",   1, false, false },
            };
            for (int i = 0; i < 4; i++) {
                int ry = netY + i * 52;
                drawCard(r, cx + 12, ry, cw - 24, 44, nets[i].connected);

                // Signal bars
                for (int b = 0; b < 4; b++) {
                    int bh = 6 + b * 5;
                    int bx2 = cx + 28 + b * 7;
                    int by2 = ry + 30 - bh;
                    Color bc = b < nets[i].bars ? Color(70,200,120,220) : Color(40,50,90,180);
                    r.drawRect(bx2, by2, 5, bh, bc);
                }

                if (sf) r.drawText(sf, nets[i].ssid, cx + 60, ry + 8, UITheme::textPrimary());
                if (sf) r.drawText(sf, nets[i].secured ? "WPA2 Secured" : "Open",
                    cx + 60, ry + 24, UITheme::textMuted());
                if (nets[i].connected && sf)
                    r.drawTextRight(sf, "Connected", cx + cw - 18, ry + 8,
                        Color(70,200,120,220));
            }
        }

        // Ethernet
        int ethY = rowY + (s_wifi ? 7 : 2) * ROW_H / 2 + 200;
        if (ethY < y + h - STATUS_H - 80) {
            drawCard(r, cx + 12, ethY, cw - 24, 56);
            r.drawFilledCircle(cx + 30, ethY + 28, 12, Color(70, 200, 120));
            if (hf) r.drawText(hf, "Ethernet", cx + 50, ethY + 8, UITheme::textPrimary());
            if (sf) r.drawText(sf, "eth0  |  192.168.1.100  |  1 Gbps", cx + 50, ethY + 26, UITheme::textMuted());
            if (sf) r.drawText(sf, "Connected", cx + 50, ethY + 40, Color(70,200,120,220));
        }
    }

    // ═══════════════════════════════════════════════════════════
    // BLUETOOTH
    // ═══════════════════════════════════════════════════════════
    if (s_cat == 4) {
        drawRow(r, hf, sf, cx, rowY, cw, ROW_H, "Bluetooth",
            "Enable Bluetooth for devices and file transfer", true);
        drawToggle(r, cx + cw - togW - 20, rowY + (ROW_H - togH) / 2, togW, togH, s_bluetooth);

        if (s_bluetooth) {
            int devY = rowY + ROW_H + 20;
            if (hf) r.drawText(hf, "Paired Devices", cx + 14, devY, UITheme::textPrimary());
            r.drawLine(cx + 14, devY + 24, cx + cw - 14, devY + 24, Color(35,48,80,120));
            devY += 32;

            struct { const char* name; const char* type; bool conn; } btDevs[] = {
                { "AirPods Pro",      "Headphones",    true  },
                { "Magic Keyboard",   "Keyboard",      true  },
                { "Magic Trackpad",   "Trackpad",      false },
                { "Sony WH-1000XM5", "Headphones",    false },
            };
            for (int i = 0; i < 4; i++) {
                int ry = devY + i * 52;
                drawCard(r, cx + 12, ry, cw - 24, 44, btDevs[i].conn);
                r.drawFilledCircle(cx + 30, ry + 22, 10,
                    btDevs[i].conn ? Color(70,160,255) : Color(50,60,100));
                if (sf) r.drawText(sf, btDevs[i].name, cx + 46, ry + 8, UITheme::textPrimary());
                if (sf) r.drawText(sf, btDevs[i].type, cx + 46, ry + 24, UITheme::textMuted());
                if (btDevs[i].conn && sf)
                    r.drawTextRight(sf, "Connected", cx + cw - 18, ry + 8, Color(70,200,120,220));
                else if (sf)
                    r.drawTextRight(sf, "Disconnect >", cx + cw - 18, ry + 8, UITheme::textMuted());
            }
        }
    }

    // ═══════════════════════════════════════════════════════════
    // POWER
    // ═══════════════════════════════════════════════════════════
    if (s_cat == 8) {
        struct { const char* title; const char* sub; const char* val; Color vcol; } prows[] = {
            { "Power Mode",       "Choose performance vs battery balance",   "Balanced",    Color(80,200,160) },
            { "Sleep after idle", "Put display to sleep when inactive",       "5 minutes",   Color(80,140,255) },
            { "Hibernate",        "Save state and power off completely",       "30 minutes",  Color(160,110,255) },
            { "Wake on LAN",      "Allow remote wake from network",            "Disabled",    Color(130,145,185) },
        };
        for (int i = 0; i < 4; i++) {
            int ry = rowY + i * ROW_H;
            drawRow(r, hf, sf, cx, ry, cw, ROW_H, prows[i].title, prows[i].sub, i % 2 == 0);
            if (sf) r.drawTextRight(sf, prows[i].val, cx + cw - 20,
                ry + 10, prows[i].vcol);
        }

        // Battery card
        int batY = rowY + 4 * ROW_H + 16;
        drawCard(r, cx + 12, batY, cw - 24, 80);
        if (hf) r.drawText(hf, "Battery Status", cx + 28, batY + 10, UITheme::textPrimary());

        // Battery bar
        int bbarX = cx + 28, bbarW = cw - 80, bbarH = 18;
        r.drawRoundedRect(bbarX, batY + 36, bbarW, bbarH, 5, Color(20, 28, 58, 220));
        r.drawRoundedRect(bbarX, batY + 36, (int)(bbarW * 0.78f), bbarH, 5, Color(70,200,120,220));
        r.drawRoundedRect(bbarX, batY + 36, (int)(bbarW * 0.78f), bbarH / 2, 5, Color(255,255,255,12));
        if (sf) r.drawText(sf, "78%  —  Plugged in, charging", bbarX, batY + 58, UITheme::textMuted());
    }

    // ═══════════════════════════════════════════════════════════
    // ABOUT
    // ═══════════════════════════════════════════════════════════
    if (s_cat == 13) {
        // OS logo area
        drawCard(r, cx + 12, rowY, cw - 24, 100);
        r.drawRoundedVerticalGradient(cx + 28, rowY + 10, 80, 80, 14,
            Color(50,80,200,220), Color(100,40,160,220));
        r.drawFilledCircle(cx + 68, rowY + 50, 28, Color(255,255,255,20));
        if (hf) r.drawTextCentered(hf, "K", cx + 28, rowY + 10, 80, 80, Color(255,255,255,240));
        if (hf) r.drawText(hf, "Kaizor OS", cx + 124, rowY + 16, UITheme::textPrimary());
        if (sf) {
            r.drawText(sf, "Version 1.0.0  (Build 2026.05.19)", cx + 124, rowY + 38, UITheme::textMuted());
            r.drawText(sf, "FreeBSD 15-CURRENT  |  x86_64", cx + 124, rowY + 54, UITheme::textMuted());
            r.drawText(sf, "KaizorDE 1.0  |  SDL2 Graphics", cx + 124, rowY + 70, UITheme::textMuted());
        }

        struct { const char* label; const char* value; } sysinfo[] = {
            { "Processor",  "Intel Core Ultra 9 285K  |  24 cores  |  5.7 GHz" },
            { "Memory",     "32 GB DDR5-6400  |  Dual Channel" },
            { "Graphics",   "KaizorGFX  |  OpenGL 4.6  |  Vulkan 1.3" },
            { "Storage",    "2 TB NVMe PCIe 5.0  +  4 TB HDD" },
            { "Kernel",     "1.0.0-RELEASE  (FreeBSD 15.0-CURRENT)" },
            { "Uptime",     "3 hours, 42 minutes" },
        };
        int infoY = rowY + 116;
        for (int i = 0; i < 6; i++) {
            int ry = infoY + i * 44;
            drawCard(r, cx + 12, ry, cw - 24, 38);
            if (sf) {
                r.drawText(sf, sysinfo[i].label, cx + 24, ry + (38 - r.measureTextH(sf,"A")*2)/2,
                    UITheme::textMuted());
                r.drawText(sf, sysinfo[i].value, cx + cw / 3, ry + (38 - r.measureTextH(sf,"A")*2)/2,
                    UITheme::textPrimary());
            }
        }
    }

    // ═══════════════════════════════════════════════════════════
    // DEFAULT for other categories
    // ═══════════════════════════════════════════════════════════
    if (s_cat == 5 || s_cat == 6 || s_cat == 7 ||
        s_cat == 9 || s_cat == 10 || s_cat == 11 || s_cat == 12) {

        static const struct { const char* t; const char* s; bool tog; } generic[][4] = {
            // Security (5)
            { {"Password Required","Require password to unlock screen",true},
              {"Screen Lock","Lock screen after 5 minutes of inactivity",true},
              {"Firewall","Block unauthorized incoming connections",true},
              {"Secure Boot","Verify system integrity at startup",true} },
            // Privacy (6)
            { {"Location Services","Allow apps to access your location",false},
              {"Camera Access","Allow apps to use the camera",false},
              {"Microphone Access","Allow apps to record audio",false},
              {"Analytics","Send crash reports to improve the OS",true} },
            // Notifications (7)
            { {"Show Notifications","Display banner alerts from apps",true},
              {"Lock Screen Notifications","Show notifications when locked",false},
              {"Do Not Disturb","Silence all notifications",false},
              {"Sound Alerts","Play audio for notifications",true} },
            // Accessibility (9)
            { {"Large Text","Increase text size throughout the system",false},
              {"High Contrast","Use high contrast colors for readability",false},
              {"Reduce Motion","Minimize animation effects",false},
              {"Screen Reader","Enable text-to-speech for accessibility",false} },
            // Users (10)
            { {"Guest Account","Allow temporary guest sessions",false},
              {"Auto Login","Log in automatically on startup",false},
              {"Show Login Screen","Display login screen on wake",true},
              {"Parental Controls","Restrict access for other users",false} },
            // Storage (11)
            { {"Auto Clean Trash","Empty trash automatically after 30 days",false},
              {"iCloud Drive","Sync files to cloud storage",false},
              {"Optimize Storage","Store items in cloud when space is low",false},
              {"Show Storage Info","Display storage usage in menu bar",true} },
            // Updates (12)
            { {"Automatic Updates","Download updates in the background",true},
              {"Security Updates","Install security patches automatically",true},
              {"Beta Releases","Receive beta and pre-release updates",false},
              {"Notify Only","Only notify, do not install automatically",false} },
        };

        int catIdx = s_cat - 5;
        if (catIdx >= 0 && catIdx < 7) {
            for (int i = 0; i < 4; i++) {
                int ry = rowY + i * ROW_H;
                drawRow(r, hf, sf, cx, ry, cw, ROW_H,
                    generic[catIdx][i].t, generic[catIdx][i].s, i % 2 == 0);
                drawToggle(r, cx + cw - togW - 20, ry + (ROW_H - togH) / 2,
                    togW, togH, generic[catIdx][i].tog);
            }
        }
    }

    // ── Status bar ────────────────────────────────────────────
    int stY = y + h - STATUS_H;
    r.drawVerticalGradient(x, stY, w, STATUS_H,
        Color(13, 17, 34, 255), Color(9, 12, 26, 255));
    r.drawLine(x, stY, x + w, stY, Color(32, 44, 78, 140));

    if (sf) {
        char lbl[48]; snprintf(lbl, sizeof(lbl), "  %s", CATS[s_cat].label);
        r.drawText(sf, lbl, x + 8,
            stY + (STATUS_H - r.measureTextH(sf, "A")) / 2, UITheme::textMuted());
        r.drawTextRight(sf, "All changes saved  ",
            x + w, stY + (STATUS_H - r.measureTextH(sf, "A")) / 2,
            Color(70, 200, 120, 180));
    }
}
