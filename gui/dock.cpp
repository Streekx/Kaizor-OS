#include "dock.hpp"
#include "taskbar.hpp"
#include "ui_theme.hpp"
#include <cmath>
#include <cstring>
#include <algorithm>

Dock::Dock() : mouseX(0), mouseY(0), hoveredItem(-1) {
    items[0] = { "Files",    Color( 80,140,255), Color( 55,110,230),  true  };
    items[1] = { "Terminal", Color( 40,200,120), Color( 28,160, 90),  true  };
    items[2] = { "Browser",  Color(255,140, 50), Color(220,110, 28),  true  };
    items[3] = { "Settings", Color(160,120,255), Color(130, 90,220),  false };
    items[4] = { "Music",    Color(255, 80,150), Color(220, 50,120),  false };
    items[5] = { "Notes",    Color(255,210, 50), Color(220,180, 28),  false };
    items[6] = { "Calendar", Color( 60,200,200), Color( 40,160,160),  false };
}

void Dock::setMousePos(int mx, int my) {
    mouseX = mx;
    mouseY = my;
}

// Smooth scale for neighboring icons (magnification effect)
static float getMagnification(int i, int hovered) {
    if (hovered < 0) return 1.0f;
    int dist = std::abs(i - hovered);
    if (dist == 0) return 1.28f;
    if (dist == 1) return 1.14f;
    if (dist == 2) return 1.06f;
    return 1.0f;
}

static float getLift(int i, int hovered) {
    if (hovered < 0) return 0.0f;
    int dist = std::abs(i - hovered);
    if (dist == 0) return 14.0f;
    if (dist == 1) return 8.0f;
    if (dist == 2) return 3.0f;
    return 0.0f;
}

// Draw a folder icon
static void drawDockFolder(Renderer& r, int x, int y, int sz, Color col, Color dark) {
    r.drawRoundedVerticalGradient(x,      y + sz/5, sz,    sz - sz/5, 10, col,  dark);
    r.drawRoundedRect(x, y, sz * 2/5, sz/5 + 4, 5,
        Color(std::min(col.r+35,255), std::min(col.g+35,255), std::min(col.b+35,255), col.a));
    r.drawRect(x + 3, y + sz/5 + 2, sz - 6, sz/7, Color(255,255,255,16));
}

// Draw a terminal icon (screen with > prompt)
static void drawDockTerminal(Renderer& r, int x, int y, int sz, Color col, Color dark, TTF_Font* sf) {
    r.drawRoundedVerticalGradient(x, y, sz, sz, 10, col, dark);
    r.drawRoundedRect(x, y, sz, sz/4, 10, Color(255,255,255,12));
    // Screen bezel
    int bw = sz - 10, bh = sz - 18;
    int bx = x + 5, by = y + 8;
    r.drawRoundedRect(bx, by, bw, bh, 4, Color(8, 10, 18, 200));
    if (sf) {
        r.drawTextCentered(sf, ">_", bx, by, bw, bh,
            Color(80, 220, 120, 220));
    }
}

// Draw a browser icon (globe)
static void drawDockBrowser(Renderer& r, int x, int y, int sz, Color col, Color dark) {
    r.drawRoundedVerticalGradient(x, y, sz, sz, 10, col, dark);
    r.drawRoundedRect(x, y, sz, sz/4, 10, Color(255,255,255,12));
    int cx = x + sz/2, cy = y + sz/2, radius = sz/3;
    r.drawCircleOutline(cx, cy, radius, Color(255,255,255,180));
    r.drawLine(cx, cy - radius, cx, cy + radius, Color(255,255,255,120));
    r.drawLine(cx - radius, cy, cx + radius, cy, Color(255,255,255,120));
    // Latitude arcs (simplified with ellipses)
    r.drawLine(cx - radius, cy - radius/3, cx + radius, cy - radius/3, Color(255,255,255,70));
    r.drawLine(cx - radius, cy + radius/3, cx + radius, cy + radius/3, Color(255,255,255,70));
}

// Draw a gear/settings icon
static void drawDockSettings(Renderer& r, int x, int y, int sz, Color col, Color dark) {
    r.drawRoundedVerticalGradient(x, y, sz, sz, 10, col, dark);
    r.drawRoundedRect(x, y, sz, sz/4, 10, Color(255,255,255,12));
    int cx = x + sz/2, cy = y + sz/2;
    int outer = sz/3, inner = sz/5;
    for (int a = 0; a < 8; a++) {
        float ang = a * 3.14159f / 4.0f;
        int gx = cx + (int)(outer * cosf(ang));
        int gy = cy + (int)(outer * sinf(ang));
        r.drawFilledCircle(gx, gy, sz/10, Color(255,255,255,200));
    }
    r.drawFilledCircle(cx, cy, inner, Color(255,255,255,220));
    r.drawFilledCircle(cx, cy, sz/9,  Color(col.r/2, col.g/2, col.b/2, 200));
}

// Draw a music note icon
static void drawDockMusic(Renderer& r, int x, int y, int sz, Color col, Color dark) {
    r.drawRoundedVerticalGradient(x, y, sz, sz, 10, col, dark);
    r.drawRoundedRect(x, y, sz, sz/4, 10, Color(255,255,255,12));
    int cx = x + sz/2, cy = y + sz/2;
    // Stem
    r.drawRect(cx - 2, cy - sz/3, 4, sz*2/3, Color(255,255,255,200));
    // Beam
    r.drawRect(cx - 2, cy - sz/3, sz/5, 3, Color(255,255,255,200));
    // Note head
    r.drawFilledEllipse(cx - sz/8, cy + sz/5, sz/9, sz/11, Color(255,255,255,210));
}

// Draw a notes/document icon
static void drawDockNotes(Renderer& r, int x, int y, int sz, Color col, Color dark) {
    r.drawRoundedVerticalGradient(x, y, sz, sz, 10, col, dark);
    r.drawRoundedRect(x, y, sz, sz/4, 10, Color(255,255,255,12));
    int px2 = x + sz/6, py2 = y + sz/5;
    int pw  = sz * 4/6, ph = sz * 3/5;
    r.drawRoundedRect(px2, py2, pw, ph, 4, Color(255,255,255,200));
    Color lc = Color(col.r/3, col.g/3, col.b/3, 200);
    r.drawRect(px2 + 4, py2 + 6,  pw - 8, 2, lc);
    r.drawRect(px2 + 4, py2 + 11, pw - 8, 2, lc);
    r.drawRect(px2 + 4, py2 + 16, pw - 12, 2, lc);
    r.drawRect(px2 + 4, py2 + 21, pw - 10, 2, lc);
}

// Draw a calendar icon
static void drawDockCalendar(Renderer& r, int x, int y, int sz, Color col, Color dark) {
    r.drawRoundedVerticalGradient(x, y, sz, sz, 10, col, dark);
    int hdr = sz / 4;
    r.drawRoundedRect(x, y, sz, hdr, 10, Color(255,255,255,25));
    // Grid lines
    int gx = x + 4, gy = y + hdr + 4, gw = sz - 8, gh = sz - hdr - 8;
    r.drawLine(gx, gy + gh/3,     gx + gw, gy + gh/3,     Color(255,255,255,50));
    r.drawLine(gx, gy + gh*2/3,   gx + gw, gy + gh*2/3,   Color(255,255,255,50));
    r.drawLine(gx + gw/3,   gy, gx + gw/3,   gy + gh, Color(255,255,255,50));
    r.drawLine(gx + gw*2/3, gy, gx + gw*2/3, gy + gh, Color(255,255,255,50));
    // Day number highlight
    r.drawRoundedRect(gx + gw/3 + 2, gy + 2, gw/3 - 4, gh/3 - 4, 3,
        Color(255,255,255,180));
    (void)dark;
}

void Dock::render(Renderer& r, TTF_Font* /*font*/, TTF_Font* smallFont) {
    const int SCRW = 1280;
    const int SCRH = 720;

    // Compute base layout
    int dockBaseY = SCRH - HEIGHT - 6;

    // Compute magnified sizes
    float mags[ITEM_COUNT];
    for (int i = 0; i < ITEM_COUNT; i++) mags[i] = getMagnification(i, hoveredItem);

    // Compute total magnified width
    float magTotal = 0;
    for (int i = 0; i < ITEM_COUNT; i++) {
        magTotal += ICON_SIZE * mags[i];
        if (i < ITEM_COUNT - 1) magTotal += ICON_GAP;
    }
    int totalW = (int)magTotal + DOCK_PADX * 2;
    int dockH  = HEIGHT + (hoveredItem >= 0 ? 8 : 0);
    int dockX  = (SCRW - totalW) / 2;
    int dockY  = dockBaseY;

    // ── Detect hover using magnified layout ───────────────────
    hoveredItem = -1;
    {
        float ix = (float)(dockX + DOCK_PADX);
        for (int i = 0; i < ITEM_COUNT; i++) {
            float sz = ICON_SIZE * mags[i];
            float lift = getLift(i, hoveredItem);
            int iy = dockY + DOCK_PADY - (int)lift;
            if (mouseX >= (int)ix && mouseX < (int)(ix + sz) &&
                mouseY >= iy        && mouseY < iy + (int)sz) {
                hoveredItem = i;
                break;
            }
            ix += sz + ICON_GAP;
        }
    }

    // Recompute magnification and layout with hoveredItem known
    for (int i = 0; i < ITEM_COUNT; i++) mags[i] = getMagnification(i, hoveredItem);
    magTotal = 0;
    for (int i = 0; i < ITEM_COUNT; i++) {
        magTotal += ICON_SIZE * mags[i];
        if (i < ITEM_COUNT - 1) magTotal += ICON_GAP;
    }
    totalW = (int)magTotal + DOCK_PADX * 2;
    dockX  = (SCRW - totalW) / 2;

    // ── Multi-layer drop shadows ──────────────────────────────
    r.drawRoundedRect(dockX + 8,  dockY + 14, totalW, dockH, 24, Color(0,0,0,80));
    r.drawRoundedRect(dockX + 4,  dockY + 8,  totalW, dockH, 24, Color(0,0,0,55));
    r.drawRoundedRect(dockX + 2,  dockY + 4,  totalW, dockH, 24, Color(0,0,0,30));

    // ── Dock glass body ───────────────────────────────────────
    r.drawRoundedVerticalGradient(dockX, dockY, totalW, dockH, 22,
        Color(22, 30, 60, 232),
        Color(14, 20, 44, 232));

    // Top edge shine (glass highlight)
    r.drawRoundedRect(dockX + 4, dockY + 3, totalW - 8, dockH / 3, 20,
        Color(255, 255, 255, 10));

    // Border with inner glow when hovering
    Color borderCol = hoveredItem >= 0
        ? Color(80, 100, 200, 120)
        : Color(55, 70, 130, 90);
    r.drawRoundedRect(dockX, dockY, totalW, dockH, 22,
        Color(borderCol.r, borderCol.g, borderCol.b, 0));
    for (int brd = 0; brd < 2; brd++) {
        r.drawRoundedRect(dockX + brd, dockY + brd,
            totalW - brd*2, dockH - brd*2, 22 - brd,
            Color(borderCol.r, borderCol.g, borderCol.b,
                  borderCol.a / 2 * (brd + 1)));
    }

    // ── Icons ─────────────────────────────────────────────────
    float iconX = (float)(dockX + DOCK_PADX);

    for (int i = 0; i < ITEM_COUNT; i++) {
        float mag  = mags[i];
        float lift = getLift(i, hoveredItem);
        bool  hov  = (hoveredItem == i);

        int sz = (int)(ICON_SIZE * mag);
        int ix = (int)iconX;
        int iy = dockY + DOCK_PADY + (ICON_SIZE - sz) - (int)lift;

        // Per-icon outer glow on hover
        if (hov) {
            r.drawFilledCircle(ix + sz/2, iy + sz/2, sz/2 + 8,
                Color(items[i].iconColor.r,
                      items[i].iconColor.g,
                      items[i].iconColor.b, 30));
            r.drawFilledCircle(ix + sz/2, iy + sz/2, sz/2 + 4,
                Color(items[i].iconColor.r,
                      items[i].iconColor.g,
                      items[i].iconColor.b, 20));
        }

        // Drop shadow
        r.drawRoundedRect(ix + sz/8, iy + sz/6, sz, sz, 12,
            Color(0, 0, 0, hov ? 100 : 70));

        // Draw icon
        switch (i) {
        case 0: drawDockFolder(r, ix, iy, sz, items[i].iconColor, items[i].glowColor); break;
        case 1: drawDockTerminal(r, ix, iy, sz, items[i].iconColor, items[i].glowColor, smallFont); break;
        case 2: drawDockBrowser(r, ix, iy, sz, items[i].iconColor, items[i].glowColor); break;
        case 3: drawDockSettings(r, ix, iy, sz, items[i].iconColor, items[i].glowColor); break;
        case 4: drawDockMusic(r, ix, iy, sz, items[i].iconColor, items[i].glowColor); break;
        case 5: drawDockNotes(r, ix, iy, sz, items[i].iconColor, items[i].glowColor); break;
        case 6: drawDockCalendar(r, ix, iy, sz, items[i].iconColor, items[i].glowColor); break;
        default: break;
        }

        // Inner shine on icon (top highlight)
        r.drawRoundedRect(ix + 2, iy + 2, sz - 4, sz / 3, 10,
            Color(255, 255, 255, hov ? 45 : 28));
        // Icon border
        for (int brd = 0; brd < 2; brd++) {
            r.drawRoundedRect(ix + brd, iy + brd, sz - brd*2, sz - brd*2, 10 - brd,
                Color(255, 255, 255, (hov ? 70 : 30) / (brd + 1)));
        }

        // Running app dot (glowing)
        if (items[i].running) {
            int dotY = dockY + dockH - 9;
            r.drawFilledCircle(ix + sz/2, dotY, 5,
                Color(items[i].iconColor.r,
                      items[i].iconColor.g,
                      items[i].iconColor.b, 60));
            r.drawFilledCircle(ix + sz/2, dotY, 3,
                UITheme::accentCyan());
        }

        // Notification badge
        if (i == 0 || i == 1) {
            int bdgX = ix + sz - 14;
            int bdgY = iy + 2;
            r.drawFilledCircle(bdgX, bdgY, 8, Color(255, 60, 60, 230));
            r.drawFilledCircle(bdgX, bdgY, 7, Color(255, 80, 80, 200));
            if (smallFont) {
                r.drawTextCentered(smallFont, i == 0 ? "3" : "1",
                    bdgX - 5, bdgY - 7, 10, 14,
                    Color(255, 255, 255, 255));
            }
        }

        // Hover tooltip
        if (hov && smallFont) {
            const char* name = items[i].name;
            int lw = r.measureTextW(smallFont, name) + 16;
            int lh = 22;
            int lx = ix + sz/2 - lw/2;
            int ly = iy - lh - 6;
            r.drawRoundedRect(lx - 1, ly - 1, lw + 2, lh + 2, 7,
                Color(items[i].iconColor.r,
                      items[i].iconColor.g,
                      items[i].iconColor.b, 80));
            r.drawRoundedRect(lx, ly, lw, lh, 6,
                Color(14, 18, 36, 230));
            r.drawTextCentered(smallFont, name, lx, ly, lw, lh,
                UITheme::textPrimary());
        }

        iconX += sz + ICON_GAP;
    }
}
