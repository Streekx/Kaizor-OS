#include "task_manager_app.hpp"
#include "../gui/ui_theme.hpp"
#include <cmath>
#include <cstring>
#include <cstdio>

struct ProcessEntry {
    const char* name;
    const char* user;
    float       cpu;
    float       mem;
    int         pid;
    const char* status;
    Color       statusCol;
};

static const ProcessEntry PROCS[] = {
    { "kaizor",        "root",   2.1f,  128.4f,  1,    "Running", Color( 70,200,120) },
    { "desktop_shell", "root",   1.4f,   64.2f,  142,  "Running", Color( 70,200,120) },
    { "window_mgr",    "root",   0.8f,   32.1f,  143,  "Running", Color( 70,200,120) },
    { "compositor",    "root",   3.2f,   88.6f,  144,  "Running", Color( 70,200,120) },
    { "audio_server",  "root",   0.2f,   18.4f,  201,  "Running", Color( 70,200,120) },
    { "network_mgr",   "root",   0.1f,    8.2f,  202,  "Running", Color( 70,200,120) },
    { "kfontd",        "root",   0.0f,    4.1f,  203,  "Idle",    Color(130,145,185) },
    { "knotifyd",      "root",   0.0f,    6.3f,  204,  "Idle",    Color(130,145,185) },
    { "syslog",        "root",   0.1f,    3.8f,  205,  "Running", Color( 70,200,120) },
    { "kernel_task",   "root",   0.5f,  512.0f,  0,    "System",  Color( 70,130,255) },
    { "terminal",      "kaizor", 0.3f,   22.4f,  310,  "Running", Color( 70,200,120) },
    { "settings",      "kaizor", 0.2f,   34.1f,  311,  "Running", Color( 70,200,120) },
    { "file_mgr",      "kaizor", 0.4f,   28.8f,  312,  "Running", Color( 70,200,120) },
    { "task_mgr",      "kaizor", 0.6f,   30.2f,  313,  "Running", Color( 70,200,120) },
    { "updater",       "root",   0.0f,   12.0f,  401,  "Sleeping",Color(255,190, 60) },
    { "cron",          "root",   0.0f,    2.1f,  402,  "Sleeping",Color(255,190, 60) },
};
static const int PROC_COUNT = 16;

static const char* TABS[] = { "Processes", "CPU", "Memory", "Disk", "Network" };
static const int   TAB_COUNT = 5;

static int   s_tab      = 0;
static int   s_selRow   = -1;
static int   s_frame    = 0;
static float s_cpuAnim[60] = {};
static float s_memAnim[60] = {};
static bool  s_anim_init = false;

static void initAnims() {
    if (s_anim_init) return;
    s_anim_init = true;
    for (int i = 0; i < 60; i++) {
        float t = i / 60.0f;
        s_cpuAnim[i] = 0.05f + 0.25f * sinf(t * 3.14159f * 3.0f + 1.2f)
                              + 0.10f * sinf(t * 3.14159f * 7.0f)
                              + 0.08f * cosf(t * 3.14159f * 5.0f);
        s_memAnim[i] = 0.38f + 0.08f * sinf(t * 3.14159f * 2.0f + 0.5f);
        if (s_cpuAnim[i] < 0) s_cpuAnim[i] = 0;
        if (s_cpuAnim[i] > 1) s_cpuAnim[i] = 1;
        if (s_memAnim[i] < 0) s_memAnim[i] = 0;
        if (s_memAnim[i] > 1) s_memAnim[i] = 1;
    }
}

TaskManagerApp::TaskManagerApp() : selectedRow(-1) {}

void TaskManagerApp::update() {
    initAnims();
    s_frame++;
}

void TaskManagerApp::handleEvent(SDL_Event& e, int x, int y, int w, int h) {
    const int TAB_H    = 44;
    const int HDR_H    = 28;
    const int ROW_H    = 32;
    const int SIDEBAR  = 64;
    (void)w;

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int mx = e.button.x, my = e.button.y;

        // Tab click
        int tabW = (w - SIDEBAR) / TAB_COUNT;
        if (my >= y && my < y + TAB_H) {
            for (int i = 0; i < TAB_COUNT; i++) {
                int tx = x + SIDEBAR + i * tabW;
                if (mx >= tx && mx < tx + tabW) {
                    s_tab = i;
                    s_selRow = -1;
                    break;
                }
            }
        }

        // Row click
        if (s_tab == 0) {
            int rowStart = y + TAB_H + HDR_H;
            int row = (my - rowStart) / ROW_H;
            if (row >= 0 && row < PROC_COUNT) {
                s_selRow = (s_selRow == row) ? -1 : row;
                selectedRow = s_selRow;
            }
        }
    }
}

static void drawSparkline(Renderer& r, int x, int y, int w, int h,
                           float* data, int count, Color col) {
    r.drawRoundedRect(x, y, w, h, 4, Color(12, 16, 30, 200));
    r.drawRoundedRect(x, y, w, h, 4, Color(40, 50, 90, 80));

    for (int i = 1; i < count; i++) {
        int x1 = x + (i - 1) * w / (count - 1);
        int y1 = y + h - (int)(data[i - 1] * (h - 4)) - 2;
        int x2 = x + i * w / (count - 1);
        int y2 = y + h - (int)(data[i] * (h - 4)) - 2;
        r.drawLine(x1, y1, x2, y2, Color(col.r, col.g, col.b, 180));
        r.drawLine(x1, y1 + 1, x2, y2 + 1, Color(col.r, col.g, col.b, 80));
    }

    // Fill area under curve
    for (int i = 0; i < count; i++) {
        int lx = x + i * w / (count - 1);
        int ly = y + h - (int)(data[i] * (h - 4)) - 2;
        r.drawLine(lx, ly, lx, y + h - 2, Color(col.r, col.g, col.b, 25));
    }
}

static void drawProgressBar(Renderer& r, int x, int y, int w, int h,
                             float val, Color col) {
    r.drawRoundedRect(x, y, w, h, h / 2, Color(18, 24, 48, 220));
    if (val > 0) {
        int fw = (int)(w * val);
        if (fw < h) fw = h;
        r.drawRoundedRect(x, y, fw, h, h / 2, col);
        r.drawRoundedRect(x, y, fw, h / 2, h / 2, Color(255, 255, 255, 18));
    }
}

static void drawUsageCard(Renderer& r, TTF_Font* font, TTF_Font* sf,
                           int x, int y, int w, int h,
                           const char* title, float pct,
                           float* spark, int sparkCount,
                           Color accentCol, const char* detail) {
    r.drawRoundedVerticalGradient(x, y, w, h, 14,
        Color(20, 26, 50, 240), Color(14, 20, 42, 240));
    r.drawRoundedRect(x, y, w, h, 14, Color(55, 70, 130, 60));
    r.drawRoundedRect(x + 2, y + 2, w - 4, h / 3, 12, Color(255, 255, 255, 5));

    // Accent top stripe
    r.drawRoundedRect(x, y, w, 3, 14,
        Color(accentCol.r, accentCol.g, accentCol.b, 200));

    int pad = 14;

    if (sf) r.drawText(sf, title, x + pad, y + 10, UITheme::textMuted());

    char pctStr[16];
    snprintf(pctStr, sizeof(pctStr), "%.0f%%", pct * 100.0f);
    if (font) r.drawText(font, pctStr, x + pad, y + 26, UITheme::textPrimary());

    if (sf && detail) r.drawText(sf, detail, x + pad, y + h - 18, UITheme::textMuted());

    // Sparkline
    int spW = w - pad * 2;
    int spH = h / 3 - 4;
    int spY = y + h - spH - 22;
    if (sparkCount > 1)
        drawSparkline(r, x + pad, spY, spW, spH, spark, sparkCount, accentCol);

    // Progress ring (simple arc via line approximation)
    int ringCX = x + w - 44;
    int ringCY = y + 28;
    int ringR  = 18;
    r.drawCircleOutline(ringCX, ringCY, ringR, Color(30, 40, 75, 200));
    int filled = (int)(pct * 360);
    for (int deg = -90; deg < -90 + filled; deg += 3) {
        float rad = deg * 3.14159f / 180.0f;
        int px2 = ringCX + (int)(ringR * cosf(rad));
        int py2 = ringCY + (int)(ringR * sinf(rad));
        r.drawFilledCircle(px2, py2, 2,
            Color(accentCol.r, accentCol.g, accentCol.b, 200));
    }
    r.drawFilledCircle(ringCX, ringCY, ringR - 6, Color(14, 20, 42, 240));
}

void TaskManagerApp::render(Renderer& r, TTF_Font* font, TTF_Font* smallFont,
                             int x, int y, int w, int h) {
    TTF_Font* sf = smallFont ? smallFont : font;
    TTF_Font* hf = font;

    const int TAB_H  = 44;
    const int HDR_H  = 28;
    const int ROW_H  = 32;
    const int STATUS_H = 24;
    const int SIDEBAR_W = 64;

    // Background
    r.drawRect(x, y, w, h, Color(10, 13, 26, 255));

    // ── Sidebar strip ─────────────────────────────────────────
    r.drawVerticalGradient(x, y, SIDEBAR_W, h,
        Color(14, 18, 36, 255), Color(10, 14, 30, 255));
    r.drawLine(x + SIDEBAR_W, y, x + SIDEBAR_W, y + h,
        Color(35, 48, 80, 150));

    struct { const char* icon; const char* label; Color col; } sideItems[] = {
        { "CPU", "Proc",  Color( 80,140,255) },
        { "MEM", "Mem",   Color(160,110,255) },
        { "DSK", "Disk",  Color( 70,200,120) },
        { "NET", "Net",   Color(255,150, 50) },
    };
    for (int i = 0; i < 4; i++) {
        int iy = y + TAB_H + 10 + i * 52;
        bool sel = (s_tab == i + 1);
        if (sel) {
            r.drawRoundedRect(x + 4, iy, SIDEBAR_W - 8, 46, 8,
                Color(40, 55, 110, 200));
            r.drawRect(x + 4, iy, 3, 46, sideItems[i].col);
        }
        r.drawFilledCircle(x + SIDEBAR_W / 2, iy + 16, 12, sideItems[i].col);
        r.drawFilledCircle(x + SIDEBAR_W / 2, iy + 16, 8,
            Color(sideItems[i].col.r / 3, sideItems[i].col.g / 3, sideItems[i].col.b / 3, 220));
        if (sf) r.drawTextCentered(sf, sideItems[i].icon,
            x + 2, iy + 8, SIDEBAR_W - 4, 16, Color(220, 230, 255, 180));
        if (sf) r.drawTextCentered(sf, sideItems[i].label,
            x + 2, iy + 30, SIDEBAR_W - 4, 12, UITheme::textMuted());
    }

    // ── Tab bar ───────────────────────────────────────────────
    int tabW = (w - SIDEBAR_W) / TAB_COUNT;
    r.drawVerticalGradient(x + SIDEBAR_W, y, w - SIDEBAR_W, TAB_H,
        Color(16, 20, 40, 255), Color(12, 16, 34, 255));
    r.drawLine(x + SIDEBAR_W, y + TAB_H, x + w, y + TAB_H, Color(35, 48, 80, 180));

    for (int i = 0; i < TAB_COUNT; i++) {
        int tx = x + SIDEBAR_W + i * tabW;
        bool active = (s_tab == i);

        if (active) {
            r.drawRoundedRect(tx + 4, y + 4, tabW - 8, TAB_H - 8, 8,
                Color(45, 62, 130, 200));
            r.drawRect(tx + 10, y + TAB_H - 3, tabW - 20, 3,
                Color(80, 140, 255, 220));
        }
        if (sf) r.drawTextCentered(sf, TABS[i], tx, y, tabW, TAB_H,
            active ? UITheme::textPrimary() : UITheme::textMuted());
    }

    int mainX = x + SIDEBAR_W;
    int mainW = w - SIDEBAR_W;

    // ── Process list tab ──────────────────────────────────────
    if (s_tab == 0) {
        // Column header
        r.drawVerticalGradient(mainX, y + TAB_H, mainW, HDR_H,
            Color(18, 23, 46, 255), Color(14, 18, 40, 255));
        r.drawLine(mainX, y + TAB_H + HDR_H, mainX + mainW, y + TAB_H + HDR_H,
            Color(35, 48, 80, 150));

        int col1 = mainX + 10;
        int col2 = mainX + mainW * 35 / 100;
        int col3 = mainX + mainW * 50 / 100;
        int col4 = mainX + mainW * 62 / 100;
        int col5 = mainX + mainW * 74 / 100;
        int col6 = mainX + mainW * 86 / 100;

        if (sf) {
            r.drawText(sf, "Process",  col1, y + TAB_H + 7, UITheme::textMuted());
            r.drawText(sf, "User",     col2, y + TAB_H + 7, UITheme::textMuted());
            r.drawText(sf, "PID",      col3, y + TAB_H + 7, UITheme::textMuted());
            r.drawText(sf, "CPU %",    col4, y + TAB_H + 7, UITheme::textMuted());
            r.drawText(sf, "MEM MB",   col5, y + TAB_H + 7, UITheme::textMuted());
            r.drawText(sf, "Status",   col6, y + TAB_H + 7, UITheme::textMuted());
        }

        // Process rows
        int rowStart = y + TAB_H + HDR_H;
        int maxRows = (h - TAB_H - HDR_H - STATUS_H) / ROW_H;

        for (int i = 0; i < PROC_COUNT && i < maxRows; i++) {
            int ry = rowStart + i * ROW_H;
            bool sel = (i == s_selRow);
            bool even = (i % 2 == 0);

            if (sel) {
                r.drawRect(mainX, ry, mainW, ROW_H, Color(50, 75, 160, 180));
                r.drawLine(mainX, ry, mainX + mainW, ry, Color(70, 100, 200, 120));
                r.drawLine(mainX, ry + ROW_H - 1, mainX + mainW, ry + ROW_H - 1,
                    Color(70, 100, 200, 120));
            } else if (even) {
                r.drawRect(mainX, ry, mainW, ROW_H, Color(14, 18, 38, 60));
            }

            // Process indicator dot
            r.drawFilledCircle(col1 + 6, ry + ROW_H / 2, 4,
                Color(PROCS[i].statusCol.r, PROCS[i].statusCol.g,
                      PROCS[i].statusCol.b, 200));

            // CPU bar mini
            int barW = 40;
            int barH = 6;
            float cpuPct = PROCS[i].cpu / 10.0f;
            if (cpuPct > 1) cpuPct = 1;
            r.drawRoundedRect(col4 - 2, ry + ROW_H / 2 - barH / 2, barW, barH, 3,
                Color(18, 24, 50, 200));
            if (cpuPct > 0) {
                Color bc = cpuPct > 0.7f ? Color(255, 80, 80) :
                           cpuPct > 0.3f ? Color(255, 190, 60) : Color(70, 200, 120);
                r.drawRoundedRect(col4 - 2, ry + ROW_H / 2 - barH / 2,
                    (int)(barW * cpuPct), barH, 3, bc);
            }

            if (sf) {
                char buf[32];
                r.drawText(sf, PROCS[i].name,
                    col1 + 16, ry + (ROW_H - r.measureTextH(sf, "A")) / 2,
                    sel ? UITheme::textPrimary() : Color(200, 212, 245, 220));
                r.drawText(sf, PROCS[i].user, col2,
                    ry + (ROW_H - r.measureTextH(sf, "A")) / 2, UITheme::textMuted());
                snprintf(buf, sizeof(buf), "%d", PROCS[i].pid);
                r.drawText(sf, buf, col3,
                    ry + (ROW_H - r.measureTextH(sf, "A")) / 2, UITheme::textMuted());
                snprintf(buf, sizeof(buf), "%.1f", PROCS[i].cpu);
                r.drawText(sf, buf, col4 + 44,
                    ry + (ROW_H - r.measureTextH(sf, "A")) / 2, UITheme::textSecondary());
                snprintf(buf, sizeof(buf), "%.1f", PROCS[i].mem);
                r.drawText(sf, buf, col5,
                    ry + (ROW_H - r.measureTextH(sf, "A")) / 2, UITheme::textSecondary());
                r.drawText(sf, PROCS[i].status, col6,
                    ry + (ROW_H - r.measureTextH(sf, "A")) / 2, PROCS[i].statusCol);
            }

            r.drawLine(mainX + 4, ry + ROW_H - 1, mainX + mainW - 4, ry + ROW_H - 1,
                Color(28, 36, 65, 80));
        }

        // Kill button if row selected
        if (s_selRow >= 0) {
            int btnW = 100, btnH = 28;
            int btnX = mainX + mainW - btnW - 16;
            int btnY = y + h - STATUS_H - btnH - 8;
            r.drawRoundedVerticalGradient(btnX, btnY, btnW, btnH, 8,
                Color(180, 50, 50, 220), Color(140, 30, 30, 220));
            r.drawRoundedRect(btnX, btnY, btnW, btnH / 2, 8, Color(255, 255, 255, 12));
            if (sf) r.drawTextCentered(sf, "Force Quit", btnX, btnY, btnW, btnH,
                Color(255, 220, 220, 240));
        }
    }

    // ── CPU tab ───────────────────────────────────────────────
    if (s_tab == 1) {
        int pad = 16;
        int cardW = (mainW - pad * 3) / 2;
        int cardH = (h - TAB_H - STATUS_H - pad * 3) / 2;

        // Scroll cpu data
        static float dispCPU[60];
        static float dispMEM[60];
        static float dispDSK[60];
        static float dispNET[60];
        static bool init2 = false;
        if (!init2) {
            init2 = true;
            for (int i = 0; i < 60; i++) {
                dispCPU[i] = s_cpuAnim[i];
                dispMEM[i] = s_memAnim[i];
                dispDSK[i] = 0.05f + 0.15f * sinf(i * 0.3f);
                dispNET[i] = 0.02f + 0.10f * sinf(i * 0.5f + 1.0f);
                if (dispDSK[i] < 0) dispDSK[i] = 0;
                if (dispNET[i] < 0) dispNET[i] = 0;
            }
        }
        // Animate last value
        float curCPU = s_cpuAnim[(s_frame / 4) % 60];
        dispCPU[59] = curCPU;

        int cx1 = mainX + pad;
        int cy1 = y + TAB_H + pad;
        drawUsageCard(r, hf, sf, cx1, cy1, cardW, cardH,
            "CPU USAGE", curCPU, dispCPU, 60,
            Color(80, 140, 255), "Intel Core Ultra 9 285K  |  16 cores");

        int cx2 = mainX + pad * 2 + cardW;
        drawUsageCard(r, hf, sf, cx2, cy1, cardW, cardH,
            "MEMORY", s_memAnim[(s_frame / 8) % 60], dispMEM, 60,
            Color(160, 110, 255), "12.4 GB / 32.0 GB used");

        int cy2 = y + TAB_H + pad * 2 + cardH;
        drawUsageCard(r, hf, sf, cx1, cy2, cardW, cardH,
            "DISK I/O", dispDSK[(s_frame / 6) % 60], dispDSK, 60,
            Color(70, 200, 120), "Read: 12.4 MB/s  |  Write: 3.2 MB/s");

        drawUsageCard(r, hf, sf, cx2, cy2, cardW, cardH,
            "NETWORK", dispNET[(s_frame / 5) % 60], dispNET, 60,
            Color(255, 150, 50), "Down: 2.4 MB/s  |  Up: 0.8 MB/s");
    }

    // ── Memory tab ────────────────────────────────────────────
    if (s_tab == 2) {
        int pad = 16;
        if (hf) r.drawText(hf, "Memory Overview", mainX + pad, y + TAB_H + pad,
            UITheme::textPrimary());

        struct { const char* label; float used; float total; Color col; const char* detail; } mems[] = {
            { "Physical RAM",  12.4f, 32.0f, Color(160,110,255), "12.4 GB of 32.0 GB used" },
            { "Swap",           0.2f,  8.0f, Color( 80,140,255), "0.2 GB of 8.0 GB used" },
            { "GPU VRAM",       2.1f,  8.0f, Color( 70,200,120), "2.1 GB of 8.0 GB used" },
            { "Shared",         1.4f, 32.0f, Color(255,150, 50), "1.4 GB shared with GPU" },
        };
        for (int i = 0; i < 4; i++) {
            int iy = y + TAB_H + pad + 40 + i * 72;
            r.drawRoundedVerticalGradient(mainX + pad, iy, mainW - pad * 2, 60, 10,
                Color(18, 24, 50, 230), Color(14, 20, 44, 230));
            r.drawRoundedRect(mainX + pad, iy, mainW - pad * 2, 60, 10, Color(50, 65, 120, 50));

            if (sf) r.drawText(sf, mems[i].label, mainX + pad + 12, iy + 8, UITheme::textSecondary());
            if (sf) r.drawText(sf, mems[i].detail, mainX + pad + 12, iy + 42, UITheme::textMuted());

            char pct[16];
            snprintf(pct, sizeof(pct), "%.0f%%", mems[i].used / mems[i].total * 100.0f);
            if (sf) r.drawTextRight(sf, pct, mainX + mainW - pad - 8, iy + 8, UITheme::textSecondary());

            drawProgressBar(r, mainX + pad + 12, iy + 24, mainW - pad * 2 - 80, 10,
                mems[i].used / mems[i].total, mems[i].col);
        }
    }

    // ── Disk tab ──────────────────────────────────────────────
    if (s_tab == 3) {
        int pad = 16;
        if (hf) r.drawText(hf, "Storage Devices", mainX + pad, y + TAB_H + pad,
            UITheme::textPrimary());

        struct { const char* name; const char* type; float used; float total;
                 Color col; const char* detail; } disks[] = {
            { "/dev/nvme0n1p1",  "NVMe SSD",  642.0f, 2048.0f, Color(70,200,120), "System  |  ext4  |  SMART: Healthy" },
            { "/dev/nvme0n1p2",  "NVMe SSD",  118.0f,  512.0f, Color(80,140,255), "Home  |  ext4  |  SMART: Healthy" },
            { "/dev/sda1",       "HDD",       1200.0f, 4096.0f, Color(255,150,50), "Data  |  ext4  |  SMART: Healthy" },
        };
        for (int i = 0; i < 3; i++) {
            int iy = y + TAB_H + pad + 40 + i * 82;
            r.drawRoundedVerticalGradient(mainX + pad, iy, mainW - pad * 2, 70, 12,
                Color(18, 24, 50, 230), Color(14, 20, 44, 230));
            r.drawRoundedRect(mainX + pad, iy, mainW - pad * 2, 70, 12, Color(50, 65, 120, 50));
            r.drawRect(mainX + pad, iy, 3, 70, disks[i].col);

            if (sf) r.drawText(sf, disks[i].name, mainX + pad + 14, iy + 8, UITheme::textPrimary());
            if (sf) r.drawText(sf, disks[i].type, mainX + pad + 14, iy + 24, UITheme::textMuted());
            if (sf) r.drawText(sf, disks[i].detail, mainX + pad + 14, iy + 52, UITheme::textMuted());

            char sizeStr[32];
            snprintf(sizeStr, sizeof(sizeStr), "%.0f / %.0f GB", disks[i].used, disks[i].total);
            if (sf) r.drawTextRight(sf, sizeStr, mainX + mainW - pad - 8, iy + 8, UITheme::textSecondary());

            drawProgressBar(r, mainX + pad + 14, iy + 38, mainW - pad * 2 - 100, 8,
                disks[i].used / disks[i].total, disks[i].col);
        }
    }

    // ── Network tab ───────────────────────────────────────────
    if (s_tab == 4) {
        int pad = 16;
        if (hf) r.drawText(hf, "Network Interfaces", mainX + pad, y + TAB_H + pad,
            UITheme::textPrimary());

        struct { const char* name; const char* type; const char* ip;
                 float down; float up; Color col; bool active; } nets[] = {
            { "eth0",  "Ethernet",  "192.168.1.100",  24.4f, 3.2f, Color( 70,200,120), true  },
            { "wlan0", "Wi-Fi",     "192.168.1.101",   2.1f, 0.4f, Color( 80,140,255), true  },
            { "lo",    "Loopback",  "127.0.0.1",       0.0f, 0.0f, Color(130,145,185), false },
        };
        for (int i = 0; i < 3; i++) {
            int iy = y + TAB_H + pad + 40 + i * 90;
            r.drawRoundedVerticalGradient(mainX + pad, iy, mainW - pad * 2, 78, 12,
                Color(18, 24, 50, 230), Color(14, 20, 44, 230));
            r.drawRoundedRect(mainX + pad, iy, mainW - pad * 2, 78, 12, Color(50, 65, 120, 50));

            // Status indicator
            r.drawFilledCircle(mainX + pad + 20, iy + 26, 7,
                nets[i].active ? nets[i].col : Color(80, 90, 120));

            if (sf) {
                r.drawText(sf, nets[i].name, mainX + pad + 36, iy + 8, UITheme::textPrimary());
                r.drawText(sf, nets[i].type, mainX + pad + 36, iy + 24, UITheme::textMuted());
                r.drawText(sf, nets[i].ip,   mainX + pad + 36, iy + 40, UITheme::textSecondary());

                char buf[48];
                snprintf(buf, sizeof(buf), "Down: %.1f MB/s", nets[i].down);
                r.drawText(sf, buf, mainX + pad + 36, iy + 56, Color(70, 200, 120, 200));
                snprintf(buf, sizeof(buf), "Up: %.1f MB/s", nets[i].up);
                r.drawText(sf, buf, mainX + mainW / 2, iy + 56, Color(80, 140, 255, 200));
            }
        }
    }

    // ── Status bar ────────────────────────────────────────────
    int stY = y + h - STATUS_H;
    r.drawVerticalGradient(x, stY, w, STATUS_H,
        Color(14, 18, 36, 255), Color(10, 14, 28, 255));
    r.drawLine(x, stY, x + w, stY, Color(35, 48, 80, 150));

    if (sf) {
        char info[64];
        snprintf(info, sizeof(info), "  %d processes  |  CPU: 8.2%%  |  Memory: 12.4 GB / 32.0 GB",
            PROC_COUNT);
        r.drawText(sf, info, x + 8,
            stY + (STATUS_H - r.measureTextH(sf, "A")) / 2, UITheme::textMuted());
        r.drawTextRight(sf, "Refresh: 2s  |  Kaizor OS  ",
            x + w, stY + (STATUS_H - r.measureTextH(sf, "A")) / 2, UITheme::textMuted());
    }
}
