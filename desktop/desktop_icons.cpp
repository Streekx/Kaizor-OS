#include "desktop_icons.hpp"
#include "../gui/ui_theme.hpp"
#include <cmath>

DesktopIcons::DesktopIcons() : mouseX(0), mouseY(0), hoveredIcon(-1) {
    const int startX = 32;
    const int startY = 64;
    const int step   = 96;

    icons[0] = {"Home",      Color( 80, 140, 255), Color(120, 170, 255), startX, startY};
    icons[1] = {"Documents", Color( 70, 190, 120), Color(100, 220, 150), startX, startY + step};
    icons[2] = {"Downloads", Color(255, 150,  50), Color(255, 180,  80), startX, startY + step * 2};
    icons[3] = {"Computer",  Color(160, 110, 255), Color(190, 140, 255), startX, startY + step * 3};
    icons[4] = {"Trash",     Color(200,  70,  70), Color(230, 100, 100), startX, startY + step * 4};
}

void DesktopIcons::setMousePos(int mx, int my) {
    mouseX = mx;
    mouseY = my;
}

void DesktopIcons::render(Renderer& r, TTF_Font* font) {
    const int ICON_W = 68;
    const int ICON_H = 68;

    hoveredIcon = -1;
    for (int i = 0; i < COUNT; i++) {
        if (mouseX >= icons[i].x && mouseX < icons[i].x + ICON_W &&
            mouseY >= icons[i].y && mouseY < icons[i].y + ICON_H) {
            hoveredIcon = i;
        }
    }

    for (int i = 0; i < COUNT; i++) {
        bool hov = (hoveredIcon == i);
        int x = icons[i].x;
        int y = icons[i].y;

        // Glow behind icon on hover
        if (hov) {
            r.drawRoundedRect(x - 4, y - 4, ICON_W + 8, ICON_H + 8, 20,
                              Color(icons[i].color.r, icons[i].color.g, icons[i].color.b, 40));
        }

        // Icon background gradient
        r.drawRoundedVerticalGradient(x, y, ICON_W, ICON_H, 16,
            icons[i].color,
            Color((int)(icons[i].color.r * 0.7f),
                  (int)(icons[i].color.g * 0.7f),
                  (int)(icons[i].color.b * 0.7f)));

        // Inner shine
        r.drawRoundedRect(x + 6, y + 5, ICON_W - 12, ICON_H / 3, 10,
                          Color(255, 255, 255, hov ? 55 : 35));

        // Icon border
        r.drawRoundedOutline(x, y, ICON_W, ICON_H, 16,
                             Color(255, 255, 255, hov ? 70 : 40));

        // Symbol inside the icon
        int cx = x + ICON_W / 2;
        int cy = y + ICON_H / 2;
        Color sym(255, 255, 255, 210);
        Color symD(255, 255, 255, 120);

        switch (i) {
        case 0: // Home - house shape
            r.drawRect(cx - 10, cy - 2,  20, 14, sym);
            r.drawLine(cx - 14, cy - 2,  cx, cy - 16, sym);
            r.drawLine(cx + 14, cy - 2,  cx, cy - 16, sym);
            r.drawRect(cx - 4,  cy + 4,  8,  10, symD);
            break;
        case 1: // Documents - stacked pages
            r.drawRect(cx - 10, cy - 12, 18, 22, sym);
            r.drawRect(cx - 8,  cy - 8,  12, 2,  symD);
            r.drawRect(cx - 8,  cy - 4,  12, 2,  symD);
            r.drawRect(cx - 8,  cy,      8,  2,  symD);
            break;
        case 2: // Downloads - arrow down
            r.drawRect(cx - 3,  cy - 12, 6, 14, sym);
            r.drawLine(cx - 9,  cy,      cx, cy + 10, sym);
            r.drawLine(cx + 9,  cy,      cx, cy + 10, sym);
            r.drawRect(cx - 12, cy + 10, 24, 3, sym);
            break;
        case 3: // Computer - monitor
            r.drawOutlineRect(cx - 12, cy - 10, 24, 18, sym, 2);
            r.drawRect(cx - 5,  cy + 8,  10, 4,  sym);
            r.drawRect(cx - 9,  cy + 12, 18, 2,  sym);
            break;
        case 4: // Trash - can shape
            r.drawRect(cx - 9,  cy - 8,  18, 18, sym);
            r.drawRect(cx - 11, cy - 10, 22, 3,  sym);
            r.drawLine(cx - 3,  cy - 5,  cx - 3, cy + 7, symD);
            r.drawLine(cx,      cy - 5,  cx,     cy + 7, symD);
            r.drawLine(cx + 3,  cy - 5,  cx + 3, cy + 7, symD);
            break;
        }

        // Label below icon
        if (font) {
            const char* name = icons[i].name;
            int lw = r.measureTextW(font, name);
            int lx = x + (ICON_W - lw) / 2;
            int ly = y + ICON_H + 4;

            // Text shadow for readability
            r.drawText(font, name, lx + 1, ly + 1, Color(0, 0, 0, 160));
            r.drawText(font, name, lx, ly,
                       hov ? UITheme::textPrimary() : UITheme::textSecondary());
        }
    }
}
