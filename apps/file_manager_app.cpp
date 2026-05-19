#include "file_manager_app.hpp"
#include "../gui/ui_theme.hpp"

static const char* SIDEBAR_ITEMS[] = {
    "Home", "Desktop", "Documents", "Downloads",
    "Music", "Pictures", "Videos"
};
static const Color SIDEBAR_ICONS[] = {
    Color( 80,140,255), Color(160,110,255), Color( 70,190,120),
    Color(255,150, 50), Color(255, 80,150), Color( 70,210,245),
    Color(255,200, 60)
};

static const struct { const char* name; Color col; } FILES[] = {
    {"Projects",  Color( 80,140,255)},
    {"Documents", Color( 70,190,120)},
    {"Downloads", Color(255,150, 50)},
    {"Music",     Color(255, 80,150)},
    {"Pictures",  Color( 70,210,245)},
    {"Videos",    Color(160,110,255)},
    {"Work",      Color(255,200, 60)},
    {"Backups",   Color(200, 90, 90)},
    {"kernel",    Color(100,160,255)},
    {"iso",       Color( 70,200,130)},
    {"notes.txt", Color(200,200,200)},
    {"README.md", Color(180,200,100)},
};

void FileManagerApp::render(Renderer& r, TTF_Font* /*font*/, TTF_Font* smallFont,
                             int x, int y, int w, int h) {
    // Background
    r.drawRect(x, y, w, h, Color(14, 18, 32, 255));

    // ─── Toolbar ──────────────────────────────────────────────
    int tbH = 36;
    r.drawVerticalGradient(x, y, w, tbH,
        Color(22, 28, 52, 255), Color(16, 20, 42, 255));
    r.drawLine(x, y + tbH, x + w, y + tbH, Color(35, 48, 80, 150));

    // Path bar
    r.drawRoundedRect(x + 6, y + 5, w - 12, 26, 6, Color(20, 26, 52, 220));
    if (smallFont) {
        r.drawText(smallFont, "  /home/user/",
                   x + 14, y + 5 + (26 - r.measureTextH(smallFont, "A")) / 2,
                   UITheme::textSecondary());
    }

    // ─── Sidebar ──────────────────────────────────────────────
    int sideW = 160;
    r.drawRect(x, y + tbH, sideW, h - tbH, Color(16, 20, 40, 255));
    r.drawLine(x + sideW, y + tbH, x + sideW, y + h, Color(35, 48, 80, 150));

    int itemH = 34;
    if (smallFont) {
        r.drawText(smallFont, "FAVORITES",
                   x + 12, y + tbH + 10, UITheme::textMuted());
    }

    for (int i = 0; i < 7; i++) {
        int iy = y + tbH + 32 + i * itemH;
        bool selected = (i == 0);

        if (selected) {
            r.drawRoundedRect(x + 6, iy - 2, sideW - 12, itemH - 2, 6,
                              Color(50, 70, 130, 180));
        }

        // Colored dot for icon
        r.drawFilledCircle(x + 18, iy + itemH / 2 - 2, 6, SIDEBAR_ICONS[i]);

        if (smallFont) {
            r.drawText(smallFont, SIDEBAR_ITEMS[i],
                       x + 30, iy + (itemH - r.measureTextH(smallFont, "A")) / 2,
                       selected ? UITheme::textPrimary() : UITheme::textSecondary());
        }
    }

    // ─── Main content area ────────────────────────────────────
    int mainX = x + sideW + 1;
    int mainW = w - sideW - 1;
    int mainY = y + tbH;
    int mainH = h - tbH;

    r.drawRect(mainX, mainY, mainW, mainH, Color(12, 16, 30, 255));

    // Column headers
    int headH = 24;
    r.drawRect(mainX, mainY, mainW, headH, Color(18, 22, 44, 255));
    if (smallFont) {
        r.drawText(smallFont, "Name",       mainX + 10, mainY + 5, UITheme::textMuted());
        r.drawText(smallFont, "Modified",   mainX + mainW / 2, mainY + 5, UITheme::textMuted());
        r.drawText(smallFont, "Size",       mainX + mainW * 3 / 4, mainY + 5, UITheme::textMuted());
    }
    r.drawLine(mainX, mainY + headH, mainX + mainW, mainY + headH, UITheme::borderSubtle());

    // File rows (first 6 as folders, rest as files)
    int rowH = 32;
    int fcount = 12;
    for (int i = 0; i < fcount; i++) {
        int ry = mainY + headH + i * rowH;
        if (ry + rowH > y + h) break;

        bool isDir = (i < 8);
        bool even  = (i % 2 == 0);

        if (even) {
            r.drawRect(mainX, ry, mainW, rowH, Color(16, 20, 40, 120));
        }

        // Folder / file icon
        Color iconC = FILES[i].col;
        r.drawRoundedRect(mainX + 8, ry + 6, 20, 20, 4, iconC);
        if (isDir) {
            // Folder tab
            r.drawRect(mainX + 8, ry + 3, 8, 4, iconC);
        } else {
            // File lines
            r.drawLine(mainX + 11, ry + 12, mainX + 23, ry + 12, Color(20, 26, 50));
            r.drawLine(mainX + 11, ry + 16, mainX + 20, ry + 16, Color(20, 26, 50));
        }

        // Name
        if (smallFont) {
            r.drawText(smallFont, FILES[i].name,
                       mainX + 34, ry + (rowH - r.measureTextH(smallFont, "A")) / 2,
                       UITheme::textPrimary());

            // Modified date
            r.drawText(smallFont, "May 18, 2026",
                       mainX + mainW / 2, ry + (rowH - r.measureTextH(smallFont, "A")) / 2,
                       UITheme::textMuted());

            // Size
            const char* sizes[] = {"—","—","—","—","—","—","—","—","12 KB","8 KB","4 KB","2 KB"};
            r.drawText(smallFont, sizes[i],
                       mainX + mainW * 3 / 4, ry + (rowH - r.measureTextH(smallFont, "A")) / 2,
                       UITheme::textMuted());
        }
    }

    // Status bar
    r.drawRect(x, y + h - 20, w, 20, Color(16, 20, 40, 255));
    r.drawLine(x, y + h - 20, x + w, y + h - 20, UITheme::borderSubtle());
    if (smallFont) {
        r.drawText(smallFont, "  12 items  —  /home/user",
                   x + 8, y + h - 16, UITheme::textMuted());
    }
}
