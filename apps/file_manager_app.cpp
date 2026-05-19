#include "file_manager_app.hpp"
#include "../gui/ui_theme.hpp"
#include <cmath>

static const struct {
    const char* name;
    Color       col;
    bool        isDir;
    const char* date;
    const char* size;
} ENTRIES[] = {
    { "Desktop",    Color( 80,140,255), true,  "May 19, 2026", "—" },
    { "Documents",  Color( 70,190,120), true,  "May 18, 2026", "—" },
    { "Downloads",  Color(255,150, 50), true,  "May 19, 2026", "—" },
    { "Music",      Color(255, 80,150), true,  "May 15, 2026", "—" },
    { "Pictures",   Color( 70,210,245), true,  "May 12, 2026", "—" },
    { "Videos",     Color(160,110,255), true,  "May 10, 2026", "—" },
    { "Projects",   Color(255,200, 60), true,  "May 19, 2026", "—" },
    { "Backups",    Color(200, 90, 90), true,  "May 08, 2026", "—" },
    { "README.md",  Color(180,200,100), false, "May 19, 2026", "4 KB"  },
    { "notes.txt",  Color(200,200,200), false, "May 17, 2026", "12 KB" },
    { "config.json",Color( 80,200,170), false, "May 16, 2026", "2 KB"  },
    { "install.sh", Color(255,160, 80), false, "May 14, 2026", "8 KB"  },
};
static const int ENTRY_COUNT = 12;

static const struct { const char* label; Color col; } SIDEBAR[] = {
    { "Home",       Color( 80,140,255) },
    { "Desktop",    Color(160,110,255) },
    { "Documents",  Color( 70,190,120) },
    { "Downloads",  Color(255,150, 50) },
    { "Music",      Color(255, 80,150) },
    { "Pictures",   Color( 70,210,245) },
    { "Videos",     Color(160,110,255) },
    { "Trash",      Color(200, 90, 90) },
};

// Draw a proper folder icon shape
static void drawFolder(Renderer& r, int x, int y, int w, int h, Color col) {
    Color light(
        std::min(col.r + 40, 255),
        std::min(col.g + 40, 255),
        std::min(col.b + 40, 255), col.a);
    Color dark(
        std::max(col.r - 30, 0),
        std::max(col.g - 30, 0),
        std::max(col.b - 30, 0), col.a);

    // Body
    r.drawRoundedVerticalGradient(x, y + 5, w, h - 5, 5, col, dark);
    // Tab (top-left flap)
    r.drawRoundedRect(x, y, w * 2 / 5, 8, 3, light);
    // Inner shine
    r.drawRect(x + 3, y + 7, w - 6, 5, Color(255, 255, 255, 18));
}

// Draw a document/file icon
static void drawFile(Renderer& r, int x, int y, int w, int h, Color col) {
    int fold = 8;
    // Body
    r.drawRoundedRect(x, y, w - fold, h, 3, Color(220, 228, 248, 210));
    // Folded corner triangle-ish
    r.drawRect(x + w - fold - 1, y, fold + 1, fold, Color(180, 190, 220, 200));
    r.drawRect(x + w - fold, y + fold, fold, h - fold, Color(200, 210, 235, 200));
    // Content lines
    Color lc = Color(col.r, col.g, col.b, 160);
    r.drawRect(x + 4, y + h / 3,     w - fold - 8, 2, lc);
    r.drawRect(x + 4, y + h / 3 + 5, w - fold - 8, 2, lc);
    r.drawRect(x + 4, y + h / 3 + 10,w - fold - 14, 2, lc);
}

void FileManagerApp::render(Renderer& r, TTF_Font* /*font*/, TTF_Font* smallFont,
                             int x, int y, int w, int h) {
    TTF_Font* sf = smallFont;
    const int TOOL_H  = 44;
    const int SIDE_W  = 180;
    const int STATUS_H = 24;
    const int HEAD_H  = 28;

    // ── Overall background ────────────────────────────────────
    r.drawRect(x, y, w, h, Color(12, 16, 30, 255));

    // ── Toolbar ───────────────────────────────────────────────
    r.drawVerticalGradient(x, y, w, TOOL_H,
        Color(20, 26, 50, 255),
        Color(15, 20, 42, 255));
    r.drawLine(x, y + TOOL_H, x + w, y + TOOL_H, Color(35, 48, 80, 160));

    // Nav buttons: Back, Forward, Up
    struct { const char* sym; bool active; } navBtns[] = {
        { "<", false }, { ">", false }, { "^", true }
    };
    int nbX = x + 8;
    for (auto& btn : navBtns) {
        int nbR = 14;
        r.drawFilledCircle(nbX + nbR, y + TOOL_H / 2, nbR,
            btn.active ? Color(40, 52, 95, 240) : Color(25, 32, 58, 200));
        r.drawCircleOutline(nbX + nbR, y + TOOL_H / 2, nbR,
            Color(55, 70, 120, 120));
        if (sf) {
            r.drawTextCentered(sf, btn.sym, nbX, y + TOOL_H / 2 - nbR,
                nbR * 2, nbR * 2,
                btn.active ? Color(200, 215, 255, 220) : Color(80, 95, 140, 180));
        }
        nbX += nbR * 2 + 6;
    }

    // Breadcrumb path
    int pathX = nbX + 6;
    int pathW = w - (pathX - x) - 200;
    r.drawRoundedRect(pathX, y + 8, pathW, TOOL_H - 16, 8,
        Color(20, 26, 52, 220));
    r.drawRoundedRect(pathX, y + 8, pathW, TOOL_H - 16, 8,
        Color(55, 70, 120, 80));
    if (sf) {
        const char* crumbs[] = { "Home", "user", "Documents" };
        int cx2 = pathX + 10;
        for (int i = 0; i < 3; i++) {
            Color cc = (i == 2)
                ? Color(200, 215, 255, 220)
                : Color(100, 115, 160, 180);
            if (sf) r.drawText(sf, crumbs[i], cx2,
                y + TOOL_H / 2 - r.measureTextH(sf, "A") / 2, cc);
            cx2 += r.measureTextW(sf, crumbs[i]) + 4;
            if (i < 2) {
                r.drawText(sf, " / ", cx2 - 4,
                    y + TOOL_H / 2 - r.measureTextH(sf, "A") / 2,
                    Color(60, 75, 115, 160));
                cx2 += r.measureTextW(sf, " / ");
            }
        }
    }

    // Search bar
    int srchX = pathX + pathW + 8;
    int srchW = w - (srchX - x) - 8;
    r.drawRoundedRect(srchX, y + 8, srchW, TOOL_H - 16, 8,
        Color(22, 28, 55, 220));
    r.drawRoundedRect(srchX, y + 8, srchW, TOOL_H - 16, 8,
        Color(55, 70, 120, 80));
    if (sf) {
        r.drawText(sf, "⌕  Search ...", srchX + 10,
            y + TOOL_H / 2 - r.measureTextH(sf, "A") / 2,
            Color(80, 95, 140, 160));
    }

    // ── Sidebar ───────────────────────────────────────────────
    r.drawVerticalGradient(x, y + TOOL_H, SIDE_W, h - TOOL_H,
        Color(16, 20, 40, 255), Color(13, 17, 35, 255));
    r.drawLine(x + SIDE_W, y + TOOL_H, x + SIDE_W, y + h,
        Color(35, 48, 80, 150));

    int sy = y + TOOL_H + 10;
    if (sf) {
        r.drawText(sf, "FAVORITES", x + 12, sy, Color(80, 95, 140, 180));
    }
    sy += 22;

    const int SB_COUNT = 8;
    for (int i = 0; i < SB_COUNT; i++) {
        int iy  = sy + i * 32;
        bool sel = (i == 0);

        if (sel) {
            r.drawRoundedRect(x + 6, iy, SIDE_W - 12, 28, 7,
                Color(45, 62, 130, 190));
            r.drawRoundedRect(x + 6, iy, SIDE_W - 12, 28, 7,
                Color(80, 100, 200, 40));
        }

        // Icon circle
        r.drawFilledCircle(x + 20, iy + 14, 8, SIDEBAR[i].col);
        r.drawFilledCircle(x + 20, iy + 14, 5,
            Color(SIDEBAR[i].col.r/2, SIDEBAR[i].col.g/2, SIDEBAR[i].col.b/2, 200));

        if (sf) {
            r.drawText(sf, SIDEBAR[i].label, x + 34, iy + 7,
                sel ? Color(220, 230, 255, 240) : Color(150, 165, 205, 200));
        }

        // Expand arrow for directories
        if (i < 7 && sf) {
            r.drawText(sf, "›", x + SIDE_W - 18, iy + 7,
                Color(70, 85, 130, 140));
        }
    }

    // Devices section
    sy += SB_COUNT * 32 + 8;
    if (sf) {
        r.drawText(sf, "DEVICES", x + 12, sy, Color(80, 95, 140, 180));
    }
    sy += 22;
    r.drawFilledCircle(x + 20, sy + 14, 8, Color(140, 150, 180));
    if (sf) {
        r.drawText(sf, "Disk  (98 GB free)", x + 34, sy + 7,
            Color(140, 155, 195, 190));
    }

    // ── Main content: grid + list ─────────────────────────────
    int mainX = x + SIDE_W + 1;
    int mainW = w - SIDE_W - 1;
    int mainY = y + TOOL_H;
    int mainH = h - TOOL_H - STATUS_H;

    r.drawRect(mainX, mainY, mainW, mainH, Color(11, 15, 28, 255));

    // Column header
    r.drawVerticalGradient(mainX, mainY, mainW, HEAD_H,
        Color(18, 22, 44, 255), Color(14, 18, 38, 255));
    r.drawLine(mainX, mainY + HEAD_H, mainX + mainW, mainY + HEAD_H,
        Color(35, 48, 80, 150));

    if (sf) {
        r.drawText(sf, "Name",     mainX + 40,             mainY + 7, UITheme::textMuted());
        r.drawText(sf, "Modified", mainX + mainW / 2,       mainY + 7, UITheme::textMuted());
        r.drawText(sf, "Size",     mainX + mainW * 3 / 4,  mainY + 7, UITheme::textMuted());
        r.drawText(sf, "Kind",     mainX + mainW * 7 / 8,  mainY + 7, UITheme::textMuted());
    }

    // File rows
    const int ROW_H   = 34;
    const int ICON_W  = 22;
    const int ICON_H  = 18;

    for (int i = 0; i < ENTRY_COUNT; i++) {
        int ry = mainY + HEAD_H + i * ROW_H;
        if (ry + ROW_H > mainY + mainH) break;

        bool even = (i % 2 == 0);
        if (even) {
            r.drawRect(mainX, ry, mainW, ROW_H, Color(14, 18, 38, 100));
        }

        // Hover highlight (simulated for row 2)
        bool hovered = (i == 2);
        if (hovered) {
            r.drawRect(mainX, ry, mainW, ROW_H, Color(50, 70, 150, 80));
            r.drawLine(mainX, ry, mainX + mainW, ry, Color(60, 80, 160, 100));
            r.drawLine(mainX, ry + ROW_H - 1, mainX + mainW, ry + ROW_H - 1,
                Color(60, 80, 160, 100));
        }

        // Icon
        int iconX = mainX + 8;
        int iconY2 = ry + (ROW_H - ICON_H) / 2;
        if (ENTRIES[i].isDir) {
            drawFolder(r, iconX, iconY2, ICON_W, ICON_H, ENTRIES[i].col);
        } else {
            drawFile(r, iconX, iconY2, ICON_W, ICON_H, ENTRIES[i].col);
        }

        // Name
        if (sf) {
            Color nameCol = hovered
                ? Color(235, 242, 255, 255)
                : UITheme::textPrimary();
            r.drawText(sf, ENTRIES[i].name,
                mainX + 36, ry + (ROW_H - r.measureTextH(sf, "A")) / 2,
                nameCol);

            r.drawText(sf, ENTRIES[i].date,
                mainX + mainW / 2, ry + (ROW_H - r.measureTextH(sf, "A")) / 2,
                UITheme::textMuted());

            r.drawText(sf, ENTRIES[i].size,
                mainX + mainW * 3 / 4, ry + (ROW_H - r.measureTextH(sf, "A")) / 2,
                UITheme::textMuted());

            const char* kind = ENTRIES[i].isDir ? "Folder" : "Document";
            r.drawText(sf, kind,
                mainX + mainW * 7 / 8, ry + (ROW_H - r.measureTextH(sf, "A")) / 2,
                UITheme::textMuted());
        }

        r.drawLine(mainX + 4, ry + ROW_H - 1, mainX + mainW - 4, ry + ROW_H - 1,
            Color(28, 36, 65, 100));
    }

    // ── Status bar ────────────────────────────────────────────
    int stY = y + h - STATUS_H;
    r.drawVerticalGradient(x, stY, w, STATUS_H,
        Color(15, 20, 40, 255), Color(11, 15, 30, 255));
    r.drawLine(x, stY, x + w, stY, Color(35, 48, 80, 150));

    if (sf) {
        r.drawText(sf, "  12 items  —  8 folders, 4 files",
            x + 8, stY + (STATUS_H - r.measureTextH(sf, "A")) / 2,
            UITheme::textMuted());

        r.drawTextRight(sf, "98.2 GB available  ",
            x + w, stY + (STATUS_H - r.measureTextH(sf, "A")) / 2,
            UITheme::textMuted());
    }
}
