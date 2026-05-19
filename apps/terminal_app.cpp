#include "terminal_app.hpp"
#include "../gui/ui_theme.hpp"
#include <cstring>

static const char* LINES[] = {
    "root@kaizor:~# uname -r",
    "kaizor-1.0.0-RELEASE",
    "root@kaizor:~# ls /boot",
    "bootloader  kernel  modules  initrd",
    "root@kaizor:~# ps aux | head -6",
    "  PID  PPID  CMD",
    "    1     0  /sbin/kaizor_init",
    "   12     1  /usr/sbin/display_server",
    "   28     1  /usr/bin/desktop_shell",
    "   45     1  /usr/bin/window_manager",
    "root@kaizor:~# df -h",
    "Filesystem      Size   Used  Avail  Use%",
    "/dev/ada0p2     120G    18G   102G   15%",
    "tmpfs           8.0G   240M   7.8G    3%",
    "root@kaizor:~# _",
};
static const int LINE_COUNT = 15;

TerminalApp::TerminalApp() : frame(0) {}

void TerminalApp::update() {
    frame++;
}

void TerminalApp::render(Renderer& r, TTF_Font* font, TTF_Font* smallFont,
                          int x, int y, int w, int h) {
    TTF_Font* tf = smallFont ? smallFont : font;

    // Terminal background - pitch black
    r.drawRect(x, y, w, h, Color(6, 8, 14, 255));

    // Left padding
    int px = x + 14;
    int py = y + 10;
    int lineH = tf ? (TTF_FontLineSkip(tf) + 2) : 18;

    for (int i = 0; i < LINE_COUNT && i < h / lineH; i++) {
        const char* line = LINES[i];
        bool isPrompt = (line[0] == 'r' && line[1] == 'o' && line[2] == 'o');
        (void)isPrompt;
        bool isCursor = (i == LINE_COUNT - 1);

        Color textColor = isPrompt ? Color(80, 220, 130) : Color(190, 205, 230);

        if (isCursor) {
            // Draw cursor blink
            if (tf) {
                int tw = r.measureTextW(tf, line);
                r.drawText(tf, line, px, py + i * lineH, Color(80, 220, 130));
                // Blinking cursor block
                if ((frame / 30) % 2 == 0) {
                    r.drawRect(px + tw, py + i * lineH, 8, lineH - 2, Color(80, 220, 130, 200));
                }
            }
        } else {
            if (tf) r.drawText(tf, line, px, py + i * lineH, textColor);
        }
    }

    // Bottom status bar
    r.drawRect(x, y + h - 22, w, 22, Color(14, 18, 30, 255));
    r.drawLine(x, y + h - 22, x + w, y + h - 22, Color(40, 55, 90, 160));
    if (smallFont) {
        r.drawText(smallFont, " root@kaizor  ~  bash  120x32 ",
                   x + 8, y + h - 18, Color(90, 120, 180));
    }
}
