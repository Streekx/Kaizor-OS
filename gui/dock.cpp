#include "dock.hpp"
#include "taskbar.hpp"
#include "ui_theme.hpp"
#include <cmath>
#include <cstring>
#include <algorithm>

// AppType values matching window_manager
static const int AT_FILES    = 2;
static const int AT_TERMINAL = 1;
static const int AT_SETTINGS = 4;
static const int AT_NOTES    = 5;
static const int AT_CALENDAR = 6;
static const int AT_TASKMAN  = 3;

Dock::Dock() : mouseX(0), mouseY(0), hoveredItem(-1) {
    items[0] = { "Files",       Color( 80,140,255), Color( 55,110,230), true,  AT_FILES    };
    items[1] = { "Terminal",    Color( 40,200,120), Color( 28,160, 90), true,  AT_TERMINAL };
    items[2] = { "Settings",    Color(160,120,255), Color(130, 90,220), false, AT_SETTINGS };
    items[3] = { "Notes",       Color(255,210, 50), Color(220,180, 28), false, AT_NOTES    };
    items[4] = { "Calendar",    Color( 60,200,200), Color( 40,160,160), false, AT_CALENDAR };
    items[5] = { "Tasks",       Color(255,120, 80), Color(220, 90, 55), false, AT_TASKMAN  };
}

void Dock::setMousePos(int mx, int my) {
    mouseX = mx;
    mouseY = my;
}

int Dock::getClickedApp(int mx, int my) {
    const int SCRW = 1280;
    const int SCRH = 720;
    int dockBaseY = SCRH - HEIGHT - 6;

    float mags[ITEM_COUNT];
    for (int i = 0; i < ITEM_COUNT; i++) mags[i] = 1.0f;

    float magTotal = 0;
    for (int i = 0; i < ITEM_COUNT; i++) {
        magTotal += ICON_SIZE * mags[i];
        if (i < ITEM_COUNT - 1) magTotal += ICON_GAP;
    }
    int totalW = (int)magTotal + DOCK_PADX * 2;
    int dockX  = (SCRW - totalW) / 2;

    float iconX = (float)(dockX + DOCK_PADX);
    for (int i = 0; i < ITEM_COUNT; i++) {
        int ix = (int)iconX;
        int iy = dockBaseY + DOCK_PADY;
        if (mx >= ix && mx < ix + ICON_SIZE && my >= iy && my < iy + ICON_SIZE)
            return items[i].appType;
        iconX += ICON_SIZE + ICON_GAP;
    }
    return -1;
}

static float getMagnification(int i, int hovered) {
    if (hovered < 0) return 1.0f;
    int dist = std::abs(i - hovered);
    if (dist == 0) return 1.30f;
    if (dist == 1) return 1.15f;
    if (dist == 2) return 1.06f;
    return 1.0f;
}

static float getLift(int i, int hovered) {
    if (hovered < 0) return 0.0f;
    int dist = std::abs(i - hovered);
    if (dist == 0) return 16.0f;
    if (dist == 1) return  9.0f;
    if (dist == 2) return  3.0f;
    return 0.0f;
}

// ── Icon draw helpers ─────────────────────────────────────────────────────────

static void drawDockFolder(Renderer& r, int x, int y, int sz, Color col, Color dark) {
    r.drawRoundedVerticalGradient(x,     y+sz/5, sz,     sz-sz/5, 10, col, dark);
    r.drawRoundedRect(x, y, sz*2/5, sz/5+4, 5,
        Color(std::min(col.r+38,255), std::min(col.g+38,255), std::min(col.b+38,255), col.a));
    r.drawRect(x+3, y+sz/5+2, sz-6, sz/6, Color(255,255,255,16));
    // Paper inside folder
    int px = x+8, pw = sz-16, py = y+sz/3+4, ph = sz/3;
    r.drawRoundedRect(px, py, pw, ph, 3, Color(230,238,255,200));
    for (int i = 0; i < 3; i++)
        r.drawRect(px+4, py+4+i*5, pw-8, 2,
            Color(dark.r/2, dark.g/2, dark.b/2, 100));
}

static void drawDockTerminal(Renderer& r, int x, int y, int sz, Color col, Color dark, TTF_Font* sf) {
    r.drawRoundedVerticalGradient(x, y, sz, sz, 12, col, dark);
    r.drawRoundedRect(x, y, sz, sz/4, 12, Color(255,255,255,14));
    // Header dots
    r.drawFilledCircle(x+8, y+sz/8+1, 4, Color(255,95,86,220));
    r.drawFilledCircle(x+17, y+sz/8+1, 4, Color(255,189,46,220));
    r.drawFilledCircle(x+26, y+sz/8+1, 4, Color(39,201,63,220));
    // Screen
    int bw=sz-10, bh=sz-sz/4-8;
    int bx=x+5, by=y+sz/4+3;
    r.drawRoundedRect(bx, by, bw, bh, 4, Color(7,10,18,210));
    if (sf) r.drawText(sf, ">_", bx+4, by+bh/2-r.measureTextH(sf,"A")/2,
        Color(70,220,120,220));
}

static void drawDockSettings(Renderer& r, int x, int y, int sz, Color col, Color dark) {
    r.drawRoundedVerticalGradient(x, y, sz, sz, 12, col, dark);
    r.drawRoundedRect(x, y, sz, sz/4, 12, Color(255,255,255,12));
    int cx2=x+sz/2, cy=y+sz/2;
    // Outer gear
    for (int a=0; a<8; a++) {
        float ang = a*3.14159f/4.0f;
        int gx=(int)(cx2+sz/3*cosf(ang)), gy=(int)(cy+sz/3*sinf(ang));
        r.drawFilledCircle(gx, gy, sz/10, Color(255,255,255,200));
    }
    r.drawFilledCircle(cx2, cy, sz/5, Color(255,255,255,220));
    r.drawFilledCircle(cx2, cy, sz/9, Color(dark.r/2, dark.g/2, dark.b/2, 210));
}

static void drawDockNotes(Renderer& r, int x, int y, int sz, Color col, Color dark) {
    r.drawRoundedVerticalGradient(x, y, sz, sz, 12, col, dark);
    r.drawRoundedRect(x, y, sz, sz/4, 12, Color(255,255,255,12));
    // Notepad
    int px=x+8, py=y+sz/4+4, pw=sz-16, ph=sz*3/5;
    r.drawRoundedRect(px, py, pw, ph, 5, Color(255,255,255,200));
    // Lines
    Color lc(dark.r/3, dark.g/3, dark.b/3, 180);
    for (int i=0; i<4; i++)
        r.drawRect(px+5, py+6+i*7, pw-10, 2, lc);
    // Corner fold
    r.drawRect(px+pw-10, py, 10, 10, Color(220,225,240,200));
    r.drawLine(px+pw-10, py, px+pw, py+10, Color(200,205,220,180));
}

static void drawDockCalendar(Renderer& r, int x, int y, int sz, Color col, Color dark) {
    r.drawRoundedVerticalGradient(x, y, sz, sz, 12, col, dark);
    int hdr=sz/4;
    r.drawRoundedRect(x, y, sz, hdr+2, 12, Color(255,255,255,25));
    // Day label at top
    int gx=x+5, gy=y+hdr+4, gw=sz-10, gh=sz-hdr-10;
    r.drawLine(gx, gy+gh/3, gx+gw, gy+gh/3, Color(255,255,255,55));
    r.drawLine(gx, gy+gh*2/3, gx+gw, gy+gh*2/3, Color(255,255,255,55));
    r.drawLine(gx+gw/3, gy, gx+gw/3, gy+gh, Color(255,255,255,55));
    r.drawLine(gx+gw*2/3, gy, gx+gw*2/3, gy+gh, Color(255,255,255,55));
    // Today highlight
    r.drawRoundedRect(gx+gw/3+2, gy+2, gw/3-4, gh/3-4, 3,
        Color(255,255,255,200));
    (void)dark;
}

static void drawDockTasks(Renderer& r, int x, int y, int sz, Color col, Color dark) {
    r.drawRoundedVerticalGradient(x, y, sz, sz, 12, col, dark);
    r.drawRoundedRect(x, y, sz, sz/4, 12, Color(255,255,255,12));
    // CPU bars chart
    int pad=8;
    int barW = (sz - pad*2) / 5 - 2;
    float levels[] = {0.7f, 0.4f, 0.85f, 0.3f, 0.6f};
    Color barCols[] = {
        Color(80,200,120,220), Color(80,140,255,220), Color(255,80,80,220),
        Color(255,190,60,220), Color(160,110,255,220)
    };
    int chartH = sz - sz/4 - 12;
    for (int i=0; i<5; i++) {
        int bx=x+pad+i*(barW+2);
        int bh=(int)(chartH*levels[i]);
        int by=y+sz-6-bh;
        r.drawRoundedRect(bx, y+sz-6-chartH, barW, chartH, 2, Color(10,14,28,180));
        r.drawRoundedRect(bx, by, barW, bh, 2, barCols[i]);
    }
}

// ─── Render ───────────────────────────────────────────────────────────────────

void Dock::render(Renderer& r, TTF_Font* /*font*/, TTF_Font* smallFont) {
    const int SCRW = 1280;
    const int SCRH = 720;

    int dockBaseY = SCRH - HEIGHT - 6;

    float mags[ITEM_COUNT];
    for (int i=0; i<ITEM_COUNT; i++) mags[i] = getMagnification(i, hoveredItem);

    float magTotal=0;
    for (int i=0; i<ITEM_COUNT; i++) {
        magTotal += ICON_SIZE * mags[i];
        if (i<ITEM_COUNT-1) magTotal += ICON_GAP;
    }
    int totalW = (int)magTotal + DOCK_PADX*2;
    int dockX  = (SCRW - totalW)/2;
    int dockH  = HEIGHT + (hoveredItem >= 0 ? 6 : 0);
    int dockY  = dockBaseY;

    // Detect hover
    hoveredItem = -1;
    {
        float ix = (float)(dockX + DOCK_PADX);
        for (int i=0; i<ITEM_COUNT; i++) {
            float sz  = ICON_SIZE * getMagnification(i, -1);
            float lift= getLift(i, -1);
            int   iy  = dockY + DOCK_PADY - (int)lift;
            if (mouseX>=(int)ix && mouseX<(int)(ix+sz) &&
                mouseY>=iy        && mouseY<iy+(int)sz) {
                hoveredItem = i; break;
            }
            ix += sz + ICON_GAP;
        }
    }

    // Recompute with real hover
    for (int i=0; i<ITEM_COUNT; i++) mags[i] = getMagnification(i, hoveredItem);
    magTotal=0;
    for (int i=0; i<ITEM_COUNT; i++) {
        magTotal += ICON_SIZE * mags[i];
        if (i<ITEM_COUNT-1) magTotal += ICON_GAP;
    }
    totalW = (int)magTotal + DOCK_PADX*2;
    dockX  = (SCRW - totalW)/2;

    // ── Multi-layer shadows ────────────────────────────────────
    r.drawRoundedRect(dockX+10, dockY+16, totalW, dockH, 26, Color(0,0,0,90));
    r.drawRoundedRect(dockX+5,  dockY+9,  totalW, dockH, 26, Color(0,0,0,60));
    r.drawRoundedRect(dockX+2,  dockY+4,  totalW, dockH, 26, Color(0,0,0,30));

    // ── Dock glass body ───────────────────────────────────────
    r.drawRoundedVerticalGradient(dockX, dockY, totalW, dockH, 24,
        Color(24, 32, 64, 238), Color(14, 20, 46, 238));

    // Top-edge glass sheen
    r.drawRoundedRect(dockX+4, dockY+3, totalW-8, dockH/3, 22,
        Color(255,255,255,11));

    // Separator line on top
    r.drawLine(dockX+16, dockY+1, dockX+totalW-16, dockY+1,
        Color(255,255,255,hoveredItem>=0 ? 16 : 8));

    // Border with glow
    Color bdCol = hoveredItem>=0 ? Color(90,115,220,120) : Color(55,72,140,80);
    for (int brd=0; brd<2; brd++) {
        r.drawRoundedRect(dockX+brd, dockY+brd,
            totalW-brd*2, dockH-brd*2, 24-brd,
            Color(bdCol.r, bdCol.g, bdCol.b, bdCol.a/(brd+1)));
    }

    // ── Icons ─────────────────────────────────────────────────
    float iconX = (float)(dockX + DOCK_PADX);

    for (int i=0; i<ITEM_COUNT; i++) {
        float mag  = mags[i];
        float lift = getLift(i, hoveredItem);
        bool  hov  = (hoveredItem == i);

        int sz2 = (int)(ICON_SIZE * mag);
        int ix  = (int)iconX;
        int iy  = dockY + DOCK_PADY + (ICON_SIZE - sz2) - (int)lift;

        // Outer glow on hover
        if (hov) {
            r.drawFilledCircle(ix+sz2/2, iy+sz2/2, sz2/2+10,
                Color(items[i].iconColor.r, items[i].iconColor.g, items[i].iconColor.b, 28));
            r.drawFilledCircle(ix+sz2/2, iy+sz2/2, sz2/2+5,
                Color(items[i].iconColor.r, items[i].iconColor.g, items[i].iconColor.b, 18));
        }

        // Drop shadow
        r.drawRoundedRect(ix+sz2/8, iy+sz2/5, sz2, sz2, 12,
            Color(0,0,0, hov ? 110 : 75));

        // Draw icon
        switch (i) {
        case 0: drawDockFolder(r,   ix, iy, sz2, items[i].iconColor, items[i].glowColor); break;
        case 1: drawDockTerminal(r, ix, iy, sz2, items[i].iconColor, items[i].glowColor, smallFont); break;
        case 2: drawDockSettings(r, ix, iy, sz2, items[i].iconColor, items[i].glowColor); break;
        case 3: drawDockNotes(r,    ix, iy, sz2, items[i].iconColor, items[i].glowColor); break;
        case 4: drawDockCalendar(r, ix, iy, sz2, items[i].iconColor, items[i].glowColor); break;
        case 5: drawDockTasks(r,    ix, iy, sz2, items[i].iconColor, items[i].glowColor); break;
        default: break;
        }

        // Top shine on icon
        r.drawRoundedRect(ix+2, iy+2, sz2-4, sz2/3, 10,
            Color(255,255,255, hov ? 48 : 28));

        // Icon border
        for (int brd=0; brd<2; brd++) {
            r.drawRoundedRect(ix+brd, iy+brd, sz2-brd*2, sz2-brd*2, 12-brd,
                Color(255,255,255, (hov ? 75 : 30)/(brd+1)));
        }

        // Running indicator dot
        if (items[i].running) {
            int dotY = dockY + dockH - 8;
            r.drawFilledCircle(ix+sz2/2, dotY, 5,
                Color(items[i].iconColor.r, items[i].iconColor.g,
                      items[i].iconColor.b, 55));
            r.drawFilledCircle(ix+sz2/2, dotY, 3, UITheme::accentCyan());
        }

        // Notification badge
        if (i == 0 || i == 1) {
            int bdgX = ix+sz2-14;
            int bdgY = iy+2;
            r.drawFilledCircle(bdgX, bdgY, 9, Color(255,55,55,235));
            r.drawFilledCircle(bdgX-2, bdgY-2, 4, Color(255,100,100,120));
            if (smallFont)
                r.drawTextCentered(smallFont, i==0?"3":"1",
                    bdgX-6, bdgY-7, 12, 14, Color(255,255,255,255));
        }

        // Tooltip
        if (hov && smallFont) {
            const char* name = items[i].name;
            int lw = r.measureTextW(smallFont, name)+20;
            int lh = 24;
            int lx = ix+sz2/2-lw/2;
            int ly = iy-lh-8;
            // Tooltip shadow
            r.drawRoundedRect(lx+2, ly+3, lw, lh, 8, Color(0,0,0,80));
            r.drawRoundedRect(lx, ly, lw, lh, 8, Color(18,24,50,240));
            r.drawRoundedRect(lx, ly, lw, lh, 8,
                Color(items[i].iconColor.r, items[i].iconColor.g,
                      items[i].iconColor.b, 80));
            r.drawRoundedRect(lx+1, ly+1, lw-2, lh/2, 7, Color(255,255,255,8));
            r.drawTextCentered(smallFont, name, lx, ly, lw, lh,
                UITheme::textPrimary());
            // Arrow
            r.drawLine(lx+lw/2-4, ly+lh, lx+lw/2, ly+lh+5, Color(18,24,50,200));
            r.drawLine(lx+lw/2,   ly+lh+5, lx+lw/2+4, ly+lh, Color(18,24,50,200));
        }

        iconX += sz2 + ICON_GAP;
    }
}
