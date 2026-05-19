#include "terminal_app.hpp"
#include "../gui/ui_theme.hpp"
#include <cstring>
#include <cmath>

struct TermLine {
    const char* text;
    int         type; // 0=prompt, 1=cmd, 2=output, 3=error, 4=success, 5=dim
};

static const TermLine LINES[] = {
    { "root@kaizor:~#",                                 0 },
    { " uname -a",                                      1 },
    { "Kaizor 1.0.0-RELEASE FreeBSD 15.0 amd64",       2 },
    { "",                                               5 },
    { "root@kaizor:~#",                                 0 },
    { " neofetch --logo",                               1 },
    { "          ██╗  ██╗",                            4 },
    { "          ██║ ██╔╝",                            4 },
    { "          █████╔╝ ",                            4 },
    { "          ██╔═██╗ ",                            4 },
    { "          ██║  ██╗",                            4 },
    { "          ╚═╝  ╚═╝",                            4 },
    { "",                                               5 },
    { "  OS:     Kaizor OS 1.0.0 x86_64",              2 },
    { "  Kernel: 1.0.0-RELEASE",                        2 },
    { "  Shell:  zsh 5.9",                              2 },
    { "  DE:     KaizorWM 1.0",                         2 },
    { "  GPU:    KaizorGFX Accelerated",               2 },
    { "  Memory: 1842 MiB / 32768 MiB",               2 },
    { "",                                               5 },
    { "root@kaizor:~#",                                 0 },
    { " ls -la",                                        1 },
    { "total 128",                                      5 },
    { "drwxr-xr-x  12 root root  4096 May 19 03:21 .", 2 },
    { "drwxr-xr-x  18 root root  4096 May 18 12:00 ..", 2 },
    { "-rw-------   1 root root  3420 May 19 03:15 .zsh_history", 2 },
    { "drwxr-xr-x   4 root root  4096 May 18 12:00 bin",  2 },
    { "drwxr-xr-x   2 root root  4096 May 18 12:00 boot", 2 },
    { "drwxr-xr-x   8 root root  4096 May 18 12:00 etc",  2 },
    { "drwxr-xr-x   6 root root  4096 May 18 12:00 home", 2 },
    { "",                                               5 },
    { "root@kaizor:~# _",                              0 },
};
static const int LINE_COUNT = 32;

static const char* TABS[] = { "  bash — root@kaizor:~  ", "  zsh — /etc/kaizor  ", "  ssh 10.0.0.2  " };
static const int   TAB_COUNT = 3;
static const int   ACTIVE_TAB = 0;

TerminalApp::TerminalApp() : frame(0) {}
void TerminalApp::update() { frame++; }

static void drawFolderIcon(Renderer& r, int x, int y, int w, int h, Color col) {
    r.drawRoundedRect(x,       y + 4, w,     h - 4, 4, col);
    r.drawRoundedRect(x,       y,     w / 3, 6,     3, Color(col.r+30, col.g+30, col.b+30, col.a));
    r.drawRect(x + 2, y + 6,   w - 4, 5,     Color(255, 255, 255, 14));
}

void TerminalApp::render(Renderer& r, TTF_Font* font, TTF_Font* smallFont,
                          int x, int y, int w, int h) {
    TTF_Font* tf = smallFont ? smallFont : font;
    const int TAB_H = 32;
    const int STATUS_H = 24;

    // ── Deep dark background ──────────────────────────────────
    r.drawRect(x, y, w, h, Color(7, 9, 15, 255));

    // ── Tab bar ───────────────────────────────────────────────
    r.drawRect(x, y, w, TAB_H, Color(11, 13, 22, 255));
    r.drawLine(x, y + TAB_H, x + w, y + TAB_H, Color(30, 40, 70, 200));

    int tabX = x + 4;
    for (int i = 0; i < TAB_COUNT; i++) {
        bool active = (i == ACTIVE_TAB);
        int tw = tf ? r.measureTextW(tf, TABS[i]) + 16 : 100;
        if (active) {
            r.drawRect(tabX, y, tw, TAB_H, Color(17, 20, 34, 255));
            r.drawRect(tabX, y, tw, 2, Color(100, 170, 255, 220));
            r.drawLine(tabX, y + TAB_H, tabX + tw, y + TAB_H, Color(17, 20, 34, 255));
        } else {
            r.drawRect(tabX, y, tw, TAB_H, Color(0, 0, 0, 0));
        }
        if (tf) {
            r.drawText(tf, TABS[i],
                tabX + 8, y + (TAB_H - r.measureTextH(tf, "A")) / 2,
                active ? Color(200, 215, 255, 240) : Color(90, 105, 145, 180));
        }
        // Close dot
        r.drawFilledCircle(tabX + tw - 10, y + TAB_H / 2, 4,
            active ? Color(80, 90, 130, 180) : Color(50, 60, 100, 120));
        tabX += tw + 1;
    }

    // New tab button
    r.drawFilledCircle(tabX + 12, y + TAB_H / 2, 9, Color(25, 30, 55, 220));
    if (tf) r.drawTextCentered(tf, "+", tabX + 3, y + 6, 18, 20, Color(120, 140, 200, 200));

    // ── Terminal content ──────────────────────────────────────
    int contentY = y + TAB_H;
    int contentH = h - TAB_H - STATUS_H;
    int px       = x + 16;
    int py       = contentY + 10;
    int lineH    = tf ? (TTF_FontLineSkip(tf) + 2) : 18;
    int maxLines = contentH / lineH;

    int start = std::max(0, LINE_COUNT - maxLines);

    for (int i = start; i < LINE_COUNT; i++) {
        int li    = i - start;
        int ly    = py + li * lineH;
        if (ly + lineH > contentY + contentH) break;

        const TermLine& line = LINES[i];
        bool isCursorLine = (i == LINE_COUNT - 1);
        bool isPromptLine = (line.type == 0 && i < LINE_COUNT - 1);

        if (isCursorLine) {
            // Prompt part
            if (tf) {
                const char* prompt = "root@kaizor:~# ";
                r.drawText(tf, prompt, px, ly, Color(80, 220, 120, 240));
                int pw = r.measureTextW(tf, prompt);
                // Blinking cursor block
                if ((frame / 28) % 2 == 0) {
                    r.drawRect(px + pw, ly, 9, lineH - 2, Color(80, 220, 120, 200));
                    // Glow
                    r.drawRect(px + pw - 2, ly - 1, 13, lineH + 1, Color(80, 220, 120, 40));
                }
            }
        } else if (isPromptLine && i + 1 < LINE_COUNT) {
            // Prompt + command on same visual line
            if (tf) {
                int pw = r.measureTextW(tf, line.text);
                r.drawText(tf, line.text, px, ly, Color(80, 220, 120, 230));
                r.drawText(tf, LINES[i+1].text, px + pw, ly, Color(220, 230, 255, 230));
            }
            i++; // skip next line since we merged it
        } else if (line.type == 0) {
            if (tf) r.drawText(tf, line.text, px, ly, Color(80, 220, 120, 230));
        } else {
            Color col;
            switch (line.type) {
                case 1: col = Color(220, 230, 255, 230); break;
                case 2: col = Color(180, 195, 230, 200); break;
                case 3: col = Color(255, 100, 100, 220); break;
                case 4: col = Color(80,  200, 140, 220); break;
                case 5: col = Color(80,   95, 135, 160); break;
                default:col = Color(180, 195, 230, 200); break;
            }
            if (tf && line.text[0] != '\0')
                r.drawText(tf, line.text, px, ly, col);
        }
    }

    // ── Status bar ────────────────────────────────────────────
    int sbY = y + h - STATUS_H;
    r.drawVerticalGradient(x, sbY, w, STATUS_H,
        Color(18, 22, 40, 255), Color(12, 16, 30, 255));
    r.drawLine(x, sbY, x + w, sbY, Color(35, 48, 80, 180));

    // Colored segments like a real terminal status bar
    struct { const char* text; Color bg; Color fg; } segments[] = {
        { " NORMAL ",   Color( 80,140,255,220), Color(10, 14, 30, 255) },
        { " root@kaizor ", Color(28, 36, 70, 200), Color(160, 175, 210, 220) },
        { " ~ ",        Color(20, 28, 58, 200), Color( 70,200,140, 220) },
        { " zsh ",      Color(16, 22, 48, 200), Color(130, 145, 185, 200) },
    };
    int segX = x + 4;
    for (auto& seg : segments) {
        if (!tf) break;
        int sw = r.measureTextW(tf, seg.text) + 4;
        r.drawRect(segX, sbY + 3, sw, STATUS_H - 6, seg.bg);
        r.drawText(tf, seg.text, segX + 2, sbY + (STATUS_H - r.measureTextH(tf, "A")) / 2,
            seg.fg);
        segX += sw + 2;
    }

    if (tf) {
        r.drawTextRight(tf, "UTF-8  LF  120×38  bash  ",
            x + w - 2, sbY + (STATUS_H - r.measureTextH(tf, "A")) / 2,
            Color(90, 105, 145, 180));
    }
    (void)drawFolderIcon;
    (void)font;
}
