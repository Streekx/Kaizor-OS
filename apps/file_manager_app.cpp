#include "file_manager_app.hpp"
#include "../gui/ui_theme.hpp"
#include <cmath>
#include <cstring>
#include <cstdio>

// ─── Static state ─────────────────────────────────────────────────────────────
static int   s_selEntry    = -1;
static int   s_sidebarSel  = 0;
static bool  s_gridView    = false;
static int   s_frame       = 0;
static bool  s_showHidden  = false;
static int   s_sortCol     = 0;

// ─── Data ─────────────────────────────────────────────────────────────────────
struct FileEntry {
    const char* name;
    Color       col;
    bool        isDir;
    const char* date;
    const char* size;
    const char* kind;
};

static const FileEntry ENTRIES[] = {
    { "Desktop",     Color( 80,140,255), true,  "Today, 09:14",     "—",      "Folder"   },
    { "Documents",   Color( 70,190,120), true,  "Yesterday",        "—",      "Folder"   },
    { "Downloads",   Color(255,150, 50), true,  "Today, 07:32",     "—",      "Folder"   },
    { "Music",       Color(255, 80,150), true,  "May 15, 2026",     "—",      "Folder"   },
    { "Pictures",    Color( 70,210,245), true,  "May 12, 2026",     "—",      "Folder"   },
    { "Videos",      Color(160,110,255), true,  "May 10, 2026",     "—",      "Folder"   },
    { "Projects",    Color(255,200, 60), true,  "Today, 03:21",     "—",      "Folder"   },
    { "Public",      Color(120,170,255), true,  "May 01, 2026",     "—",      "Folder"   },
    { "README.md",   Color(180,200,100), false, "Today, 03:20",     "4.2 KB", "Document" },
    { "notes.txt",   Color(200,210,230), false, "May 17, 2026",     "12 KB",  "Text"     },
    { "config.json", Color( 80,200,170), false, "May 16, 2026",     "2.1 KB", "JSON"     },
    { "install.sh",  Color(255,160, 80), false, "May 14, 2026",     "8.4 KB", "Script"   },
    { "kaizor.ttf",  Color(255,120,200), false, "May 10, 2026",     "286 KB", "Font"     },
    { "wallpaper.png",Color(100,180,255),false, "May 08, 2026",     "1.4 MB", "Image"    },
};
static const int ENTRY_COUNT = 14;

static const struct { const char* label; Color col; bool isSection; } SIDEBAR[] = {
    { "FAVORITES",   Color(0,0,0,0),       true  },
    { "Home",        Color( 80,140,255),   false },
    { "Desktop",     Color(160,110,255),   false },
    { "Documents",   Color( 70,190,120),   false },
    { "Downloads",   Color(255,150, 50),   false },
    { "Pictures",    Color( 70,210,245),   false },
    { "Music",       Color(255, 80,150),   false },
    { "Videos",      Color(140, 90,255),   false },
    { "DEVICES",     Color(0,0,0,0),       true  },
    { "System SSD",  Color(140,150,180),   false },
    { "Home HDD",    Color(100,160,220),   false },
    { "NETWORK",     Color(0,0,0,0),       true  },
    { "Network",     Color( 80,160,255),   false },
    { "Trash",       Color(200, 90, 90),   false },
};
static const int SIDEBAR_COUNT = 14;

// ─── Helpers ──────────────────────────────────────────────────────────────────
FileManagerApp::FileManagerApp() {}
void FileManagerApp::update() { s_frame++; }

static void drawFolderIcon(Renderer& r, int x, int y, int w, int h, Color col) {
    Color light(std::min(col.r+40,255), std::min(col.g+40,255), std::min(col.b+40,255), col.a);
    Color dark (std::max(col.r-30, 0),  std::max(col.g-30, 0),  std::max(col.b-30, 0),  col.a);
    r.drawRoundedVerticalGradient(x, y+h/5, w, h-h/5, 5, col, dark);
    r.drawRoundedRect(x, y, w*2/5, h/5+3, 3, light);
    r.drawRect(x+3, y+h/5+2, w-6, h/5, Color(255,255,255,18));
}

static void drawFileIcon(Renderer& r, int x, int y, int w, int h, Color col) {
    int fold = w/4;
    r.drawRoundedRect(x, y, w-fold, h, 3, Color(220,228,248,210));
    r.drawRect(x+w-fold-1, y, fold+1, fold, Color(180,190,220,200));
    r.drawRect(x+w-fold,   y+fold, fold, h-fold, Color(200,210,235,200));
    Color lc(col.r, col.g, col.b, 160);
    for (int i = 0; i < 3; i++)
        r.drawRect(x+3, y+h/3+i*5, w-fold-6-i*4, 2, lc);
}

static void drawImageIcon(Renderer& r, int x, int y, int w, int h, Color col) {
    r.drawRoundedVerticalGradient(x, y, w, h, 3,
        Color(col.r/2, col.g/2, col.b/2, 200), Color(20,28,55,200));
    r.drawRoundedRect(x, y, w, h, 3, Color(col.r, col.g, col.b, 60));
    // Sun
    r.drawFilledCircle(x+w*2/3, y+h/3, h/5, Color(255,220,80,200));
    // Mountain
    for (int i = 0; i < w-2; i++) {
        int mh = (int)(h/2 * sinf((float)i / (w-2) * 3.14159f));
        r.drawLine(x+1+i, y+h-2-mh, x+1+i, y+h-2, Color(col.r, col.g, col.b, 120));
    }
}

void FileManagerApp::handleEvent(SDL_Event& e, int x, int y, int w, int h) {
    const int TOOL_H   = 48;
    const int SIDE_W   = 190;
    const int STATUS_H = 26;
    const int HEAD_H   = 28;
    const int ROW_H    = 36;
    (void)w;

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int mx = e.button.x, my = e.button.y;

        // View toggle (grid/list)
        if (mx >= x + w - 80 && mx < x + w - 40 && my >= y && my < y + TOOL_H)
            s_gridView = false;
        if (mx >= x + w - 40 && mx < x + w - 8  && my >= y && my < y + TOOL_H)
            s_gridView = true;

        // Sidebar click
        int sideIdx = -1;
        int sideY = y + TOOL_H + 14;
        for (int i = 0; i < SIDEBAR_COUNT; i++) {
            if (SIDEBAR[i].isSection) { sideY += 22; continue; }
            if (mx >= x+4 && mx < x+SIDE_W-4 && my >= sideY && my < sideY+30) {
                sideIdx = i; break;
            }
            sideY += 32;
        }
        if (sideIdx >= 0) { s_sidebarSel = sideIdx; s_selEntry = -1; return; }

        // File row click
        if (!s_gridView) {
            int mainX = x + SIDE_W + 1;
            int mainY = y + TOOL_H + HEAD_H;
            int row = (my - mainY) / ROW_H;
            if (mx >= mainX && my >= mainY && row >= 0 && row < ENTRY_COUNT)
                s_selEntry = (s_selEntry == row) ? -1 : row;
        } else {
            int mainX = x + SIDE_W + 1;
            int mainY = y + TOOL_H;
            int pad = 12;
            int iconW = 88, iconH = 96;
            int perRow = (w - SIDE_W - pad * 2) / (iconW + pad);
            if (perRow < 1) perRow = 1;
            for (int i = 0; i < ENTRY_COUNT; i++) {
                int col2 = i % perRow;
                int row2 = i / perRow;
                int ix2 = mainX + pad + col2 * (iconW + pad);
                int iy2 = mainY + pad + row2 * (iconH + pad + 20);
                if (mx >= ix2 && mx < ix2 + iconW && my >= iy2 && my < iy2 + iconH + 20)
                    s_selEntry = (s_selEntry == i) ? -1 : i;
            }
        }
    }
}

void FileManagerApp::render(Renderer& r, TTF_Font* /*font*/, TTF_Font* smallFont,
                              int x, int y, int w, int h) {
    TTF_Font* sf = smallFont;
    const int TOOL_H   = 48;
    const int SIDE_W   = 190;
    const int STATUS_H = 26;
    const int HEAD_H   = 28;
    const int ROW_H    = 36;

    // ── Background ────────────────────────────────────────────
    r.drawRect(x, y, w, h, Color(10, 13, 26, 255));

    // ── Toolbar ───────────────────────────────────────────────
    r.drawVerticalGradient(x, y, w, TOOL_H,
        Color(16, 21, 46, 255), Color(12, 16, 38, 255));
    r.drawLine(x, y+TOOL_H, x+w, y+TOOL_H, Color(32,44,80,160));

    // Nav buttons: Back, Forward, Up
    struct { const char* s; bool active; } navBtns[] = {
        {"<",false},{">",false},{"^",true}
    };
    int nbX = x + 10;
    for (auto& btn : navBtns) {
        int nbR = 14;
        r.drawFilledCircle(nbX+nbR, y+TOOL_H/2, nbR,
            btn.active ? Color(42,56,108,230) : Color(22,28,56,180));
        r.drawCircleOutline(nbX+nbR, y+TOOL_H/2, nbR, Color(50,65,110,100));
        r.drawFilledCircle(nbX+nbR, y+TOOL_H/2-4, nbR/2+1, Color(255,255,255,btn.active?12:6));
        if (sf) r.drawTextCentered(sf, btn.s, nbX, y+TOOL_H/2-nbR, nbR*2, nbR*2,
            btn.active ? Color(200,215,255,210) : Color(70,85,130,160));
        nbX += nbR*2 + 6;
    }

    // Breadcrumb
    int pathX = nbX + 8;
    int pathW = w - (pathX-x) - 220;
    r.drawRoundedRect(pathX, y+8, pathW, TOOL_H-16, 8, Color(18,24,50,210));
    r.drawRoundedRect(pathX, y+8, pathW, TOOL_H-16, 8, Color(50,65,110,70));
    r.drawRoundedRect(pathX+1, y+9, pathW-2, (TOOL_H-16)/2, 8, Color(255,255,255,4));
    if (sf) {
        const char* crumbs[] = {"Home","user"};
        const char* activeC  = "Documents";
        int cx2 = pathX+10;
        for (int i = 0; i < 2; i++) {
            r.drawText(sf, crumbs[i], cx2, y+TOOL_H/2-r.measureTextH(sf,"A")/2,
                Color(100,115,160,170));
            cx2 += r.measureTextW(sf, crumbs[i]) + 2;
            r.drawText(sf, " / ", cx2, y+TOOL_H/2-r.measureTextH(sf,"A")/2,
                Color(55,70,110,140));
            cx2 += r.measureTextW(sf, " / ");
        }
        r.drawText(sf, activeC, cx2, y+TOOL_H/2-r.measureTextH(sf,"A")/2,
            Color(200,215,255,215));
    }

    // Search
    int srchX = pathX+pathW+8;
    int srchW = w-(srchX-x)-8;
    r.drawRoundedRect(srchX, y+8, srchW, TOOL_H-16, 8, Color(20,26,52,210));
    r.drawRoundedRect(srchX, y+8, srchW, TOOL_H-16, 8, Color(50,65,110,70));
    r.drawFilledCircle(srchX+15, y+TOOL_H/2, 7, Color(0,0,0,0));
    r.drawCircleOutline(srchX+15, y+TOOL_H/2, 7, Color(80,95,145,160));
    r.drawLine(srchX+20, y+TOOL_H/2+5, srchX+24, y+TOOL_H/2+9, Color(80,95,145,160));
    if (sf) r.drawText(sf, "Search ...", srchX+28,
        y+TOOL_H/2-r.measureTextH(sf,"A")/2, Color(75,90,140,150));

    // Command toolbar (second row)
    int toolY = y + TOOL_H;
    // We'll put the command bar inside the main area header but draw action buttons
    // along the right side of the breadcrumb area
    struct { const char* label; Color col; } actions[] = {
        {"+ New",    Color(70,200,120)},
        {"Copy",     Color(80,140,255)},
        {"Paste",    Color(160,110,255)},
        {"Rename",   Color(255,190,60)},
        {"Delete",   Color(255,80,80)},
    };
    // Draw view toggle buttons (list / grid)
    int vtX = x + w - 82;
    r.drawRoundedRect(vtX, y+9, 36, TOOL_H-18, 7,
        s_gridView ? Color(22,28,55,200) : Color(42,56,110,220));
    r.drawRoundedRect(vtX+38, y+9, 36, TOOL_H-18, 7,
        s_gridView ? Color(42,56,110,220) : Color(22,28,55,200));
    // List icon
    for (int i = 0; i < 3; i++)
        r.drawRect(vtX+8, y+14+i*6, 20, 2, s_gridView ? Color(70,85,130,150) : Color(200,215,255,200));
    // Grid icon
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            r.drawRect(vtX+38+7+i*9, y+14+j*8, 6, 5,
                s_gridView ? Color(200,215,255,200) : Color(70,85,130,150));

    // ── Sidebar ───────────────────────────────────────────────
    r.drawVerticalGradient(x, y+TOOL_H, SIDE_W, h-TOOL_H,
        Color(13,17,36,255), Color(10,14,30,255));
    r.drawLine(x+SIDE_W, y+TOOL_H, x+SIDE_W, y+h, Color(32,44,80,150));

    int sideY = y + TOOL_H + 12;
    for (int i = 0; i < SIDEBAR_COUNT; i++) {
        if (SIDEBAR[i].isSection) {
            if (sf) r.drawText(sf, SIDEBAR[i].label, x+12, sideY,
                Color(75,90,135,170));
            sideY += 22;
            continue;
        }
        bool sel = (i == s_sidebarSel);
        if (sel) {
            r.drawRoundedRect(x+5, sideY, SIDE_W-10, 28, 7, Color(38,54,120,190));
            r.drawRect(x+5, sideY, 3, 28, SIDEBAR[i].col);
            r.drawRoundedRect(x+5, sideY, SIDE_W-10, 14, 7, Color(255,255,255,4));
        }
        r.drawFilledCircle(x+21, sideY+14, 8, SIDEBAR[i].col);
        r.drawFilledCircle(x+21, sideY+14, 4,
            Color(SIDEBAR[i].col.r/3, SIDEBAR[i].col.g/3, SIDEBAR[i].col.b/3, 220));
        if (sf) r.drawText(sf, SIDEBAR[i].label, x+34, sideY+(28-r.measureTextH(sf,"A"))/2,
            sel ? Color(220,232,255,240) : Color(145,160,205,195));
        sideY += 32;
    }

    // ── Main file area ────────────────────────────────────────
    int mainX = x + SIDE_W + 1;
    int mainW = w - SIDE_W - 1;
    int mainY = y + TOOL_H;
    int mainH = h - TOOL_H - STATUS_H;

    r.drawRect(mainX, mainY, mainW, mainH, Color(10,13,26,255));

    if (!s_gridView) {
        // List view
        // Column header
        r.drawVerticalGradient(mainX, mainY, mainW, HEAD_H,
            Color(16,20,42,255), Color(12,16,36,255));
        r.drawLine(mainX, mainY+HEAD_H, mainX+mainW, mainY+HEAD_H,
            Color(32,44,80,150));

        int col2 = mainX + mainW*36/100;
        int col3 = mainX + mainW*58/100;
        int col4 = mainX + mainW*74/100;
        int col5 = mainX + mainW*88/100;

        if (sf) {
            r.drawText(sf, "Name",     mainX+38,  mainY+7, UITheme::textMuted());
            r.drawText(sf, "Modified", col2,       mainY+7, UITheme::textMuted());
            r.drawText(sf, "Size",     col3,       mainY+7, UITheme::textMuted());
            r.drawText(sf, "Kind",     col4,       mainY+7, UITheme::textMuted());
        }

        // Rows
        int listY = mainY + HEAD_H;
        for (int i = 0; i < ENTRY_COUNT; i++) {
            int ry = listY + i*ROW_H;
            if (ry+ROW_H > mainY+mainH) break;
            bool sel = (i == s_selEntry);
            bool even = (i%2==0);

            if (sel) {
                r.drawRect(mainX, ry, mainW, ROW_H, Color(42,64,150,180));
                r.drawLine(mainX, ry, mainX+mainW, ry, Color(60,90,190,120));
                r.drawLine(mainX, ry+ROW_H-1, mainX+mainW, ry+ROW_H-1, Color(60,90,190,120));
            } else if (even) {
                r.drawRect(mainX, ry, mainW, ROW_H, Color(13,17,38,70));
            }

            // Icon
            int iconX = mainX+8;
            int iconY2 = ry+(ROW_H-20)/2;
            if (ENTRIES[i].isDir)
                drawFolderIcon(r, iconX, iconY2, 24, 20, ENTRIES[i].col);
            else if (strstr(ENTRIES[i].name, ".png") || strstr(ENTRIES[i].name, ".jpg"))
                drawImageIcon(r, iconX, iconY2, 24, 20, ENTRIES[i].col);
            else
                drawFileIcon(r, iconX, iconY2, 24, 20, ENTRIES[i].col);

            if (sf) {
                r.drawText(sf, ENTRIES[i].name, mainX+38,
                    ry+(ROW_H-r.measureTextH(sf,"A"))/2,
                    sel ? UITheme::textPrimary() : Color(200,212,245,215));
                r.drawText(sf, ENTRIES[i].date, col2,
                    ry+(ROW_H-r.measureTextH(sf,"A"))/2, UITheme::textMuted());
                r.drawText(sf, ENTRIES[i].size, col3,
                    ry+(ROW_H-r.measureTextH(sf,"A"))/2, UITheme::textMuted());
                r.drawText(sf, ENTRIES[i].kind, col4,
                    ry+(ROW_H-r.measureTextH(sf,"A"))/2, UITheme::textMuted());
            }
            r.drawLine(mainX+4, ry+ROW_H-1, mainX+mainW-4, ry+ROW_H-1, Color(26,34,62,80));
        }
    } else {
        // Grid view
        const int ICON_W = 88, ICON_H = 70, LABEL_H = 24;
        const int PAD    = 14;
        int perRow = (mainW - PAD) / (ICON_W + PAD);
        if (perRow < 1) perRow = 1;

        r.pushClip(mainX, mainY, mainW, mainH);
        for (int i = 0; i < ENTRY_COUNT; i++) {
            int col3 = i % perRow;
            int row3 = i / perRow;
            int ix = mainX + PAD + col3*(ICON_W+PAD);
            int iy = mainY + PAD + row3*(ICON_H+LABEL_H+PAD);
            if (iy+ICON_H+LABEL_H > mainY+mainH) break;

            bool sel = (i == s_selEntry);
            if (sel) {
                r.drawRoundedRect(ix-4, iy-4, ICON_W+8, ICON_H+LABEL_H+8, 10,
                    Color(42,64,150,180));
            }

            // Icon background
            r.drawRoundedVerticalGradient(ix, iy, ICON_W, ICON_H, 12,
                Color(22,28,56,220), Color(16,22,46,220));
            r.drawRoundedRect(ix, iy, ICON_W, ICON_H, 12, Color(50,65,110,50));
            r.drawRoundedRect(ix+2, iy+2, ICON_W-4, ICON_H/3, 10, Color(255,255,255,5));

            // Draw icon content centered
            int icX = ix+ICON_W/2-16;
            int icY = iy+ICON_H/2-14;
            if (ENTRIES[i].isDir)
                drawFolderIcon(r, icX, icY, 32, 28, ENTRIES[i].col);
            else if (strstr(ENTRIES[i].name, ".png") || strstr(ENTRIES[i].name, ".jpg"))
                drawImageIcon(r, icX, icY, 32, 28, ENTRIES[i].col);
            else
                drawFileIcon(r, icX, icY, 32, 28, ENTRIES[i].col);

            // Label
            if (sf) r.drawTextCentered(sf, ENTRIES[i].name,
                ix, iy+ICON_H+2, ICON_W, LABEL_H,
                sel ? UITheme::textPrimary() : UITheme::textSecondary());
        }
        r.popClip();
    }

    // Detail panel for selected file
    if (s_selEntry >= 0 && s_selEntry < ENTRY_COUNT) {
        const FileEntry& fe = ENTRIES[s_selEntry];
        // Draw at right edge
        int pnW = 190;
        if (mainW > 500) {
            int pnX = mainX + mainW - pnW;
            r.drawRoundedVerticalGradient(pnX, mainY+4, pnW-4, mainH-8, 12,
                Color(16,20,44,230), Color(12,16,38,230));
            r.drawRoundedRect(pnX, mainY+4, pnW-4, mainH-8, 12, Color(50,65,110,60));

            // Icon preview
            int iconSz = 56;
            int iconX2 = pnX+(pnW-4-iconSz)/2;
            int iconY2 = mainY+20;
            r.drawRoundedRect(iconX2, iconY2, iconSz, iconSz, 12, Color(22,28,56,220));
            r.drawRoundedRect(iconX2, iconY2, iconSz, iconSz, 12, Color(50,65,110,50));
            if (fe.isDir)
                drawFolderIcon(r, iconX2+8, iconY2+10, iconSz-16, iconSz-20, fe.col);
            else
                drawFileIcon(r, iconX2+8, iconY2+10, iconSz-16, iconSz-20, fe.col);

            if (sf) {
                r.drawTextCentered(sf, fe.name, pnX, iconY2+iconSz+6, pnW-4, 20,
                    UITheme::textPrimary());

                struct { const char* label; const char* value; } info[] = {
                    { "Kind",     fe.kind },
                    { "Size",     fe.size },
                    { "Modified", fe.date },
                };
                int infoY = iconY2+iconSz+32;
                for (auto& inf : info) {
                    r.drawText(sf, inf.label, pnX+12, infoY, UITheme::textMuted());
                    r.drawText(sf, inf.value, pnX+12, infoY+14, UITheme::textSecondary());
                    infoY += 36;
                    r.drawLine(pnX+8, infoY-2, pnX+pnW-16, infoY-2, Color(32,42,72,100));
                }
            }
        }
    }

    // ── Status bar ────────────────────────────────────────────
    int stY = y + h - STATUS_H;
    r.drawVerticalGradient(x, stY, w, STATUS_H,
        Color(13,17,36,255), Color(9,12,26,255));
    r.drawLine(x, stY, x+w, stY, Color(32,44,80,150));

    if (sf) {
        char info[64];
        snprintf(info, sizeof(info), "  %d items  —  8 folders, 6 files", ENTRY_COUNT);
        r.drawText(sf, info, x+8, stY+(STATUS_H-r.measureTextH(sf,"A"))/2, UITheme::textMuted());
        r.drawTextRight(sf, "98.2 GB available  ",
            x+w, stY+(STATUS_H-r.measureTextH(sf,"A"))/2, UITheme::textMuted());
    }
}
