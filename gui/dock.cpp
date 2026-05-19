#include "dock.hpp"
#include "taskbar.hpp"
#include "ui_theme.hpp"
#include <cmath>
#include <cstring>

Dock::Dock() : mouseX(0), mouseY(0), hoveredItem(-1) {
    items[0] = {"Files",    Color( 80, 140, 255), Color( 60, 110, 220),  true};
    items[1] = {"Terminal", Color( 40, 200, 120), Color( 30, 160,  90),  true};
    items[2] = {"Browser",  Color(255, 140,  50), Color(220, 110,  30),  false};
    items[3] = {"Settings", Color(160, 120, 255), Color(130,  90, 220),  false};
    items[4] = {"Music",    Color(255,  80, 150), Color(220,  50, 120),  false};
    items[5] = {"Notes",    Color(255, 210,  50), Color(220, 180,  30),  false};
    items[6] = {"Calendar", Color( 60, 200, 200), Color( 40, 160, 160),  false};
}

void Dock::setMousePos(int mx, int my) {
    mouseX = mx;
    mouseY = my;
}

void Dock::render(Renderer& r, TTF_Font* /*font*/, TTF_Font* smallFont) {
    const int SCRW = 1280;
    const int SCRH = 720;
    (void)Taskbar::HEIGHT;

    int totalW = ITEM_COUNT * (ICON_SIZE + ICON_GAP) - ICON_GAP + DOCK_PADX * 2;
    int dockX  = (SCRW - totalW) / 2;
    int dockY  = SCRH - HEIGHT - 4;

    // ─── Detect hover ─────────────────────────────────────────
    hoveredItem = -1;
    for (int i = 0; i < ITEM_COUNT; i++) {
        int ix = dockX + DOCK_PADX + i * (ICON_SIZE + ICON_GAP);
        int iy = dockY + DOCK_PADY;
        if (mouseX >= ix && mouseX < ix + ICON_SIZE &&
            mouseY >= iy && mouseY < iy + ICON_SIZE) {
            hoveredItem = i;
            break;
        }
    }

    // ─── Drop shadow ──────────────────────────────────────────
    r.drawRoundedRect(dockX + 6, dockY + 10, totalW, HEIGHT,
                      22, Color(0, 0, 0, 90));
    r.drawRoundedRect(dockX + 3, dockY + 6,  totalW, HEIGHT,
                      22, Color(0, 0, 0, 50));

    // ─── Glass body ───────────────────────────────────────────
    r.drawRoundedVerticalGradient(dockX, dockY, totalW, HEIGHT, 22,
        Color(26, 34, 62, 228),
        Color(16, 22, 44, 228));

    // Glass shine (top highlight)
    r.drawRoundedRect(dockX + 4, dockY + 3, totalW - 8, HEIGHT / 3, 18,
                      Color(255, 255, 255, 12));

    // Border
    r.drawRoundedOutline(dockX, dockY, totalW, HEIGHT, 22,
                         UITheme::dockBorder());

    // ─── Icons ────────────────────────────────────────────────
    for (int i = 0; i < ITEM_COUNT; i++) {
        bool hov = (hoveredItem == i);

        int baseSize = ICON_SIZE;
        int ix = dockX + DOCK_PADX + i * (ICON_SIZE + ICON_GAP);
        int iy = dockY + DOCK_PADY;

        // Hover: lift the icon upward slightly
        if (hov) {
            iy -= 6;
        }

        // Outer shadow
        r.drawRoundedRect(ix + 3, iy + 5, baseSize, baseSize, 14,
                          Color(0, 0, 0, 80));

        // Icon body gradient
        Color iconTop(
            (int)(items[i].iconColor.r * 0.95f),
            (int)(items[i].iconColor.g * 0.95f),
            (int)(items[i].iconColor.b * 0.95f),
            hov ? 255 : 235
        );
        Color iconBot(
            (int)(items[i].glowColor.r * 0.85f),
            (int)(items[i].glowColor.g * 0.85f),
            (int)(items[i].glowColor.b * 0.85f),
            hov ? 255 : 235
        );
        r.drawRoundedVerticalGradient(ix, iy, baseSize, baseSize, 14, iconTop, iconBot);

        // Inner shine (top-left corner glow)
        r.drawRoundedRect(ix + 4, iy + 4, baseSize - 8, baseSize / 3, 10,
                          Color(255, 255, 255, hov ? 50 : 30));

        // Icon border
        r.drawRoundedOutline(ix, iy, baseSize, baseSize, 14,
                             Color(255, 255, 255, hov ? 60 : 30));

        // Draw a simple symbol inside each icon
        int cx = ix + baseSize / 2;
        int cy = iy + baseSize / 2;
        Color symColor(255, 255, 255, hov ? 230 : 200);
        Color symDim(255, 255, 255, 120);

        switch (i) {
        case 0: // Files - folder shape
            r.drawRect(cx - 12, cy - 6, 24, 14, symColor);
            r.drawRect(cx - 12, cy - 10, 10, 5,  symColor);
            break;
        case 1: // Terminal - >_ symbol
            if (smallFont) {
                r.drawTextCentered(smallFont, ">_", ix, iy, baseSize, baseSize, symColor);
            } else {
                r.drawRect(cx - 10, cy - 1, 8, 2, symColor);
                r.drawRect(cx,      cy - 4, 10, 2, symColor);
                r.drawRect(cx,      cy + 2, 10, 2, symColor);
            }
            break;
        case 2: // Browser - globe circle
            r.drawCircleOutline(cx, cy, 10, symColor);
            r.drawLine(cx - 10, cy, cx + 10, cy, symDim);
            r.drawLine(cx, cy - 10, cx, cy + 10, symDim);
            break;
        case 3: // Settings - gear dot
            r.drawFilledCircle(cx, cy, 6, symColor);
            r.drawFilledCircle(cx, cy, 3, iconBot);
            for (int a = 0; a < 8; a++) {
                float ang = a * 3.14159f / 4.0f;
                int gx = cx + (int)(10 * cos(ang));
                int gy = cy + (int)(10 * sin(ang));
                r.drawFilledCircle(gx, gy, 2, symColor);
            }
            break;
        case 4: // Music - note
            r.drawRect(cx - 2, cy - 10, 4, 14, symColor);
            r.drawRect(cx + 2,  cy - 10, 8, 3, symColor);
            r.drawFilledCircle(cx, cy + 4, 4, symColor);
            break;
        case 5: // Notes - paper lines
            r.drawRect(cx - 9, cy - 10, 18, 20, symColor);
            r.drawRect(cx - 6, cy - 6,  12, 2,  iconBot);
            r.drawRect(cx - 6, cy - 2,  12, 2,  iconBot);
            r.drawRect(cx - 6, cy + 2,  8,  2,  iconBot);
            break;
        case 6: // Calendar - grid
            r.drawOutlineRect(cx - 10, cy - 8, 20, 18, symColor);
            r.drawLine(cx - 10, cy - 2, cx + 10, cy - 2, symDim);
            r.drawLine(cx,       cy - 8, cx,      cy + 10, symDim);
            r.drawLine(cx - 10,  cy + 4, cx + 10, cy + 4, symDim);
            break;
        }

        // Running dot (bottom center)
        if (items[i].running) {
            r.drawFilledCircle(ix + baseSize / 2, dockY + HEIGHT - 8, 3,
                               UITheme::accentCyan());
        }

        // Tooltip label on hover
        if (hov && smallFont) {
            const char* name = items[i].name;
            int lw = r.measureTextW(smallFont, name);
            int lx = ix + (baseSize - lw) / 2;
            int ly = iy - 22;
            r.drawRoundedRect(lx - 6, ly - 3, lw + 12, 20, 6,
                              Color(10, 14, 30, 220));
            r.drawText(smallFont, name, lx, ly, UITheme::textPrimary());
        }
    }
}
