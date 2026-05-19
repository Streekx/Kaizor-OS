#include "notification_manager.hpp"
#include "../gui/ui_theme.hpp"
#include <cmath>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <iomanip>

// ─── Helpers ──────────────────────────────────────────────────────────────────

static std::string nowTime() {
    time_t t = time(nullptr);
    struct tm* tm2 = localtime(&t);
    std::ostringstream o;
    o << std::setw(2) << std::setfill('0') << tm2->tm_hour
      << ":" << std::setw(2) << std::setfill('0') << tm2->tm_min;
    return o.str();
}

static void drawIcon(Renderer& r, TTF_Font* sf, int cx, int cy, int rad,
                     NotifCategory cat, Color col) {
    // Outer glow
    r.drawFilledCircle(cx, cy, rad + 4,
        Color(col.r, col.g, col.b, 35));
    r.drawFilledCircle(cx, cy, rad, col);
    r.drawFilledCircle(cx, cy - rad/3, rad/2,
        Color(255, 255, 255, 28));

    // Symbol inside
    Color sym(255, 255, 255, 230);
    switch (cat) {
    case NotifCategory::SYSTEM:
        // Gear dots
        for (int i = 0; i < 6; i++) {
            float a = i * 3.14159f / 3.0f;
            r.drawFilledCircle(cx + (int)(rad*0.55f*cosf(a)),
                               cy + (int)(rad*0.55f*sinf(a)), 2, sym);
        }
        r.drawFilledCircle(cx, cy, rad/3, sym);
        r.drawFilledCircle(cx, cy, rad/5,
            Color(col.r/2, col.g/2, col.b/2, 220));
        break;

    case NotifCategory::MESSAGE:
        // Speech bubble outline
        r.drawRoundedRect(cx - rad/2, cy - rad/3, rad, rad*2/3, 3, sym);
        r.drawLine(cx - rad/4, cy + rad/3, cx - rad/2, cy + rad*2/3, sym);
        break;

    case NotifCategory::ALERT:
        // Exclamation
        r.drawRect(cx - 1, cy - rad/2, 3, rad*2/3, sym);
        r.drawFilledCircle(cx, cy + rad/3, 2, sym);
        break;

    case NotifCategory::UPDATE:
        // Down arrow
        r.drawLine(cx, cy - rad/2, cx, cy + rad/4, sym);
        r.drawLine(cx, cy + rad/4, cx - rad/3, cy - rad/6, sym);
        r.drawLine(cx, cy + rad/4, cx + rad/3, cy - rad/6, sym);
        break;

    case NotifCategory::DOWNLOAD:
        // Down arrow + tray
        r.drawLine(cx, cy - rad/2, cx, cy + rad/6, sym);
        r.drawLine(cx, cy + rad/6, cx - rad/4, cy - rad/8, sym);
        r.drawLine(cx, cy + rad/6, cx + rad/4, cy - rad/8, sym);
        r.drawRect(cx - rad/2, cy + rad/3, rad, 2, sym);
        break;

    default: // APP — letter
        if (sf) r.drawTextCentered(sf, "A",
            cx - rad/2, cy - rad/2, rad, rad, sym);
        break;
    }
}

// ─── Constructor ──────────────────────────────────────────────────────────────

NotificationManager::NotificationManager()
    : historyOpen(false), historySlide(0.0f),
      nextId(1), mouseX(0), mouseY(0), frameTime(0.0f)
{}

// ─── Push ─────────────────────────────────────────────────────────────────────

void NotificationManager::push(const std::string& appName,
                                const std::string& title,
                                const std::string& body,
                                NotifCategory cat,
                                Color iconColor,
                                bool pinned) {
    // Cap active toasts at MAX_VISIBLE — move oldest to history
    int active = 0;
    for (auto& n : notifications)
        if (!n.dismissed) active++;

    if (active >= MAX_VISIBLE) {
        for (auto& n : notifications) {
            if (!n.dismissed) {
                n.dismissed = true;
                history.insert(history.begin(), n);
                break;
            }
        }
    }

    Notification n;
    n.id          = nextId++;
    n.appName     = appName;
    n.title       = title;
    n.body        = body;
    n.timeStr     = nowTime();
    n.category    = cat;
    n.iconColor   = iconColor;
    n.read        = false;
    n.dismissed   = false;
    n.slideX      = 1.0f;   // start off-screen to the right
    n.lifeTimer   = pinned ? 999.0f : 6.0f;
    n.opacity     = 0.0f;
    n.isPinned    = pinned;
    notifications.push_back(n);
}

// ─── Seed ─────────────────────────────────────────────────────────────────────

void NotificationManager::seedDemoNotifications() {
    // Add a few "already read" entries straight to history
    auto addHist = [&](const char* app, const char* title, const char* body,
                        NotifCategory cat, Color col, const char* time2) {
        Notification n;
        n.id = nextId++;
        n.appName   = app;
        n.title     = title;
        n.body      = body;
        n.timeStr   = time2;
        n.category  = cat;
        n.iconColor = col;
        n.read      = true;
        n.dismissed = true;
        n.slideX    = 0.0f;
        n.lifeTimer = 0.0f;
        n.opacity   = 1.0f;
        n.isPinned  = false;
        history.push_back(n);
    };

    addHist("Kaizor OS",    "System Ready",       "Desktop environment loaded successfully.",
            NotifCategory::SYSTEM,   Color(80,200,120),  "09:00");
    addHist("Update Manager","Updates Available",  "3 system packages ready to install.",
            NotifCategory::UPDATE,   Color(80,140,255),  "09:02");
    addHist("File Manager", "Download Complete",   "wallpaper.png saved to Downloads.",
            NotifCategory::DOWNLOAD, Color(70,210,245),  "09:05");
    addHist("Messages",     "New message",         "Hey, are you coming to the meeting?",
            NotifCategory::MESSAGE,  Color(255,80,150),  "09:12");
    addHist("Security",     "Firewall Active",     "Incoming connections are being blocked.",
            NotifCategory::ALERT,    Color(255,140,50),  "09:14");

    // Immediately push a few live toasts that will be visible on boot
    push("Kaizor OS",     "Welcome back!",
         "Kaizor OS 1.0 — session started.",
         NotifCategory::SYSTEM,   Color(80,140,255), false);
    push("Update Manager","Updates Available",
         "3 packages ready to install.",
         NotifCategory::UPDATE,   Color(70,200,120), false);
    push("Messages",      "New message",
         "Hey, the build finished! Ready for review.",
         NotifCategory::MESSAGE,  Color(255,80,150), false);
}

// ─── Counts ───────────────────────────────────────────────────────────────────

int NotificationManager::unreadCount() const {
    int c = 0;
    for (auto& n : notifications) if (!n.read && !n.dismissed) c++;
    for (auto& n : history)       if (!n.read) c++;
    return c;
}

int NotificationManager::activeCount() const {
    int c = 0;
    for (auto& n : notifications) if (!n.dismissed) c++;
    return c;
}

// ─── toggleHistory ────────────────────────────────────────────────────────────

void NotificationManager::toggleHistory() {
    historyOpen = !historyOpen;
    if (historyOpen) {
        // Mark all active and history as read when opening tray
        for (auto& n : notifications) n.read = true;
        for (auto& n : history)       n.read = true;
    }
}

// ─── handleEvent ──────────────────────────────────────────────────────────────

void NotificationManager::handleEvent(SDL_Event& e) {
    if (e.type == SDL_MOUSEMOTION) {
        mouseX = e.motion.x;
        mouseY = e.motion.y;
    }

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int mx = e.button.x, my = e.button.y;

        // Dismiss X button on toasts
        int liveCount = 0;
        for (auto& n : notifications) if (!n.dismissed) liveCount++;

        int idx = 0;
        for (auto& n : notifications) {
            if (n.dismissed) continue;
            int toastX = SCREEN_W - TOAST_W - 12;
            int toastY = SCREEN_H - DOCK_H - TOAST_PAD
                       - (liveCount - idx) * (TOAST_H + TOAST_PAD);

            // X button top-right of toast
            int xbx = toastX + TOAST_W - 24;
            int xby = toastY + 10;
            int dx = mx - (xbx + 8), dy = my - (xby + 8);
            if (dx*dx + dy*dy <= 10*10) {
                n.dismissed = true;
                history.insert(history.begin(), n);
                idx++;
                continue;
            }

            // Action buttons: "Open" and "Dismiss"
            int btnY = toastY + TOAST_H - 28;
            int openX = toastX + TOAST_W/2 + 4;
            int dimX  = toastX + 52;

            if (my >= btnY && my < btnY + 24 && mx >= dimX && mx < dimX + 90) {
                n.dismissed = true;
                history.insert(history.begin(), n);
            }
            if (my >= btnY && my < btnY + 24 && mx >= openX && mx < openX + 90) {
                n.read = true;
                n.dismissed = true;
                history.insert(history.begin(), n);
            }

            idx++;
        }

        // History panel: "Clear All" and individual dismiss
        if (historyOpen) {
            int panelW = 380;
            int panelX = SCREEN_W - panelW - 4;
            int panelY = TASKBAR_H + 4;

            // Clear All
            int clearX = panelX + panelW - 80, clearY = panelY + 10;
            if (mx >= clearX && mx < clearX + 72 && my >= clearY && my < clearY + 26) {
                history.clear();
                notifications.erase(
                    std::remove_if(notifications.begin(), notifications.end(),
                                   [](const Notification& n){ return n.dismissed; }),
                    notifications.end());
                return;
            }

            // Individual clear
            int hItemH = 82;
            int hStartY = panelY + 54;
            for (int i = 0; i < (int)history.size(); i++) {
                int hy = hStartY + i * (hItemH + 6);
                if (hy + hItemH > panelY + 520) break;
                int hxBtn = panelX + panelW - 32;
                int dyBtn = my - (hy + 14);
                int dxBtn = mx - (hxBtn + 10);
                if (dxBtn*dxBtn + dyBtn*dyBtn <= 9*9) {
                    history.erase(history.begin() + i);
                    return;
                }
            }

            // Close panel if clicked outside
            if (!(mx >= panelX && mx < panelX + panelW &&
                  my >= panelY && my < panelY + 540)) {
                historyOpen = false;
            }
        }

        // Remove fully dismissed from active list
        notifications.erase(
            std::remove_if(notifications.begin(), notifications.end(),
                [](const Notification& n){ return n.dismissed && n.slideX >= 0.99f; }),
            notifications.end());
    }
}

// ─── update ───────────────────────────────────────────────────────────────────

void NotificationManager::update(float dt) {
    frameTime += dt;

    // Animate history panel slide
    float panelSpeed = 8.0f * dt;
    if (historyOpen)
        historySlide = std::min(1.0f, historySlide + panelSpeed);
    else
        historySlide = std::max(0.0f, historySlide - panelSpeed);

    // Animate each toast
    for (auto& n : notifications) {
        float speed = 10.0f * dt;

        if (!n.dismissed) {
            // Slide in
            n.slideX = std::max(0.0f, n.slideX - speed);
            n.opacity = std::min(1.0f, n.opacity + speed * 2.0f);

            // Count down life
            n.lifeTimer -= dt;
            if (n.lifeTimer <= 0.0f && !n.isPinned) {
                n.dismissed = true;
                history.insert(history.begin(), n);
            }
        } else {
            // Slide out
            n.slideX  = std::min(1.0f, n.slideX  + speed);
            n.opacity = std::max(0.0f, n.opacity - speed * 2.0f);
        }
    }

    // Prune fully out toasts
    notifications.erase(
        std::remove_if(notifications.begin(), notifications.end(),
            [](const Notification& n){ return n.dismissed && n.slideX >= 0.99f; }),
        notifications.end());

    // Cap history size
    if ((int)history.size() > 40)
        history.resize(40);
}

// ─── renderToast ─────────────────────────────────────────────────────────────

void NotificationManager::renderToast(Renderer& r, TTF_Font* font, TTF_Font* sf,
                                       const Notification& n, int x, int y) const {
    int alpha = (int)(n.opacity * 245);
    if (alpha <= 0) return;

    float ease = n.slideX * n.slideX * (3.0f - 2.0f * n.slideX);
    int   ox   = (int)(ease * (TOAST_W + 20));  // offset from right

    int tx = x + ox;  // slides from right

    // ── Shadow ────────────────────────────────────────────────
    r.drawRoundedRect(tx + 8,  y + 8,  TOAST_W, TOAST_H, 14, Color(0, 0, 0, alpha/3));
    r.drawRoundedRect(tx + 4,  y + 4,  TOAST_W, TOAST_H, 14, Color(0, 0, 0, alpha/5));

    // ── Body ──────────────────────────────────────────────────
    r.drawRoundedVerticalGradient(tx, y, TOAST_W, TOAST_H, 14,
        Color(20, 26, 52, alpha),
        Color(14, 20, 44, alpha));
    r.drawRoundedRect(tx, y, TOAST_W, TOAST_H, 14,
        Color(n.iconColor.r, n.iconColor.g, n.iconColor.b, alpha/4));
    // Top shine
    r.drawRoundedRect(tx + 2, y + 2, TOAST_W - 4, TOAST_H / 3, 13,
        Color(255, 255, 255, alpha/20));
    // Left accent stripe
    r.drawRoundedRect(tx, y, 4, TOAST_H, 14,
        Color(n.iconColor.r, n.iconColor.g, n.iconColor.b, alpha));

    // ── Life timer bar ────────────────────────────────────────
    if (!n.isPinned && n.lifeTimer < 6.0f) {
        float pct = std::max(0.0f, n.lifeTimer / 6.0f);
        r.drawRoundedRect(tx + 4, y + TOAST_H - 4, TOAST_W - 8, 3, 2,
            Color(22, 28, 56, alpha));
        if (pct > 0.0f) {
            Color lc = pct > 0.5f ? n.iconColor : Color(255, 120, 60);
            r.drawRoundedRect(tx + 4, y + TOAST_H - 4,
                (int)((TOAST_W - 8) * pct), 3, 2,
                Color(lc.r, lc.g, lc.b, alpha));
        }
    }

    // ── Icon ──────────────────────────────────────────────────
    drawIcon(r, sf, tx + 28, y + TOAST_H/2 - 8, 16, n.category, n.iconColor);

    // ── App name + time ───────────────────────────────────────
    if (sf) {
        r.drawText(sf, n.appName,
            tx + 52, y + 10,
            Color(n.iconColor.r + 30 > 255 ? 255 : n.iconColor.r + 30,
                  n.iconColor.g + 30 > 255 ? 255 : n.iconColor.g + 30,
                  n.iconColor.b + 30 > 255 ? 255 : n.iconColor.b + 30,
                  alpha));
        r.drawTextRight(sf, n.timeStr,
            tx + TOAST_W - 36, y + 10,
            Color(90, 105, 160, alpha));
    }

    // ── Title ─────────────────────────────────────────────────
    if (font) {
        r.drawText(font, n.title,
            tx + 52, y + 26,
            Color(210, 222, 255, alpha));
    }

    // ── Body text ─────────────────────────────────────────────
    if (sf) {
        // Truncate body to fit
        std::string bodyTxt = n.body;
        if ((int)bodyTxt.size() > 42) bodyTxt = bodyTxt.substr(0, 40) + "…";
        r.drawText(sf, bodyTxt,
            tx + 52, y + 42,
            Color(145, 158, 205, alpha));
    }

    // ── X dismiss button ──────────────────────────────────────
    int xbx = tx + TOAST_W - 24;
    int xby = y + 10;
    bool xHov = (mouseX >= xbx && mouseX < xbx + 20 &&
                 mouseY >= xby && mouseY < xby + 20);
    if (xHov) {
        r.drawFilledCircle(xbx + 9, xby + 9, 9,
            Color(255, 80, 80, alpha * 3/4));
    }
    r.drawLine(xbx + 5, xby + 5,  xbx + 13, xby + 13,
        Color(180, 190, 230, xHov ? alpha : alpha/2));
    r.drawLine(xbx + 13, xby + 5, xbx + 5,  xby + 13,
        Color(180, 190, 230, xHov ? alpha : alpha/2));

    // ── Action buttons ────────────────────────────────────────
    int btnY = y + TOAST_H - 28;
    // "Dismiss" button
    int dimX = tx + 52;
    bool dimHov = (mouseX >= dimX && mouseX < dimX + 80 &&
                   mouseY >= btnY && mouseY < btnY + 22);
    r.drawRoundedRect(dimX, btnY, 80, 22, 6,
        dimHov ? Color(45, 58, 110, alpha) : Color(25, 32, 65, alpha*3/4));
    r.drawRoundedRect(dimX, btnY, 80, 22, 6, Color(50, 65, 115, alpha/3));
    if (sf) r.drawTextCentered(sf, "Dismiss", dimX, btnY, 80, 22,
        Color(dimHov ? 200 : 140, dimHov ? 212 : 155, 255, alpha));

    // "Open" button
    int openX = tx + 52 + 88;
    bool openHov = (mouseX >= openX && mouseX < openX + 80 &&
                    mouseY >= btnY && mouseY < btnY + 22);
    r.drawRoundedRect(openX, btnY, 80, 22, 6,
        openHov ? Color(n.iconColor.r/2, n.iconColor.g/2, n.iconColor.b/2, alpha)
                : Color(25, 32, 65, alpha*3/4));
    r.drawRoundedRect(openX, btnY, 80, 22, 6,
        Color(n.iconColor.r, n.iconColor.g, n.iconColor.b, alpha/3));
    if (sf) r.drawTextCentered(sf, "Open", openX, btnY, 80, 22,
        Color(n.iconColor.r + 50 > 255 ? 255 : n.iconColor.r + 50,
              n.iconColor.g + 50 > 255 ? 255 : n.iconColor.g + 50,
              n.iconColor.b + 50 > 255 ? 255 : n.iconColor.b + 50,
              alpha));
}

// ─── renderHistoryPanel ───────────────────────────────────────────────────────

void NotificationManager::renderHistoryPanel(Renderer& r, TTF_Font* font, TTF_Font* sf) {
    if (historySlide <= 0.01f) return;

    float ease = historySlide * historySlide * (3.0f - 2.0f * historySlide);
    int alpha  = (int)(ease * 255);

    int panelW = 380;
    int panelH = 540;
    int panelX = SCREEN_W - panelW - 4;
    int panelY = TASKBAR_H + 4;

    // Slide down from taskbar
    int offY = (int)((1.0f - ease) * (panelH + 20));
    panelY -= offY;

    // ── Shadow ────────────────────────────────────────────────
    r.drawRoundedRect(panelX + 8, panelY + 12, panelW, panelH, 16, Color(0,0,0,alpha/3));
    r.drawRoundedRect(panelX + 4, panelY + 6,  panelW, panelH, 16, Color(0,0,0,alpha/5));

    // ── Panel body ────────────────────────────────────────────
    r.drawRoundedVerticalGradient(panelX, panelY, panelW, panelH, 16,
        Color(18, 22, 48, alpha),
        Color(12, 16, 38, alpha));
    r.drawRoundedRect(panelX, panelY, panelW, panelH, 16,
        Color(60, 75, 140, alpha/4));
    // Top shine
    r.drawRoundedRect(panelX + 2, panelY + 2, panelW - 4, panelH / 4, 15,
        Color(255, 255, 255, alpha/20));

    // ── Header ────────────────────────────────────────────────
    r.drawRoundedRect(panelX, panelY, panelW, 46, 16,
        Color(22, 28, 56, alpha));
    r.drawLine(panelX + 8, panelY + 46, panelX + panelW - 8, panelY + 46,
        Color(40, 52, 95, alpha/2));

    if (font) r.drawText(font, "Notifications",
        panelX + 16, panelY + 12,
        Color(210, 222, 255, alpha));

    int unread = unreadCount();
    if (unread > 0 && sf) {
        char ub[8]; snprintf(ub, sizeof(ub), "%d", unread);
        int bw = r.measureTextW(sf, ub) + 14;
        r.drawRoundedRect(panelX + 130, panelY + 12, bw, 22, 7,
            Color(255, 70, 70, alpha));
        r.drawTextCentered(sf, ub, panelX + 130, panelY + 12, bw, 22,
            Color(255, 255, 255, alpha));
    }

    // "Clear All" button
    bool clrHov = (mouseX >= panelX + panelW - 82 &&
                   mouseX <  panelX + panelW - 10 &&
                   mouseY >= panelY + 10 &&
                   mouseY <  panelY + 36);
    r.drawRoundedRect(panelX + panelW - 82, panelY + 10, 72, 26, 7,
        clrHov ? Color(50, 65, 130, alpha) : Color(28, 35, 72, alpha));
    r.drawRoundedRect(panelX + panelW - 82, panelY + 10, 72, 26, 7,
        Color(60, 75, 140, alpha/3));
    if (sf) r.drawTextCentered(sf, "Clear All",
        panelX + panelW - 82, panelY + 10, 72, 26,
        Color(clrHov ? 210 : 150, clrHov ? 220 : 165, 255, alpha));

    // ── Active notifications section ──────────────────────────
    int curY = panelY + 54;

    // Active toasts summary
    int liveCount = 0;
    for (auto& n : notifications) if (!n.dismissed) liveCount++;

    if (liveCount > 0) {
        if (sf) {
            char lb[32]; snprintf(lb, sizeof(lb), "ACTIVE  (%d)", liveCount);
            r.drawText(sf, lb, panelX + 14, curY,
                Color(80, 140, 255, alpha * 3/4));
        }
        curY += 20;

        for (auto& n : notifications) {
            if (n.dismissed) continue;
            if (curY + 66 > panelY + panelH - 10) break;

            // Mini active item
            r.drawRoundedVerticalGradient(panelX + 8, curY, panelW - 16, 60, 10,
                Color(22, 30, 62, alpha), Color(16, 22, 50, alpha));
            r.drawRoundedRect(panelX + 8, curY, panelW - 16, 60, 10,
                Color(n.iconColor.r, n.iconColor.g, n.iconColor.b, alpha/5));
            r.drawRect(panelX + 8, curY, 3, 60,
                Color(n.iconColor.r, n.iconColor.g, n.iconColor.b, alpha));

            drawIcon(r, sf, panelX + 26, curY + 30, 12, n.category, n.iconColor);

            if (font) r.drawText(font, n.title,
                panelX + 44, curY + 8,
                Color(210, 222, 255, alpha));
            if (sf) {
                std::string body = n.body;
                if ((int)body.size() > 38) body = body.substr(0, 36) + "…";
                r.drawText(sf, body, panelX + 44, curY + 26,
                    Color(130, 145, 195, alpha));
                r.drawTextRight(sf, n.timeStr,
                    panelX + panelW - 14, curY + 8,
                    Color(80, 95, 150, alpha));
            }

            // Pulsing ring on active items
            float pulse = 0.6f + 0.4f * sinf(frameTime * 3.0f);
            r.drawCircleOutline(panelX + 26, curY + 30, 13,
                Color(n.iconColor.r, n.iconColor.g, n.iconColor.b,
                      (uint8_t)(alpha/3 * pulse)));

            curY += 66;
        }

        r.drawLine(panelX + 10, curY, panelX + panelW - 10, curY,
            Color(40, 52, 95, alpha/2));
        curY += 8;
    }

    // ── History section ───────────────────────────────────────
    if (sf) r.drawText(sf, history.empty() ? "HISTORY  (empty)" : "HISTORY",
        panelX + 14, curY,
        Color(90, 105, 160, alpha * 3/4));
    curY += 20;

    if (history.empty()) {
        if (sf) r.drawTextCentered(sf, "No past notifications",
            panelX, curY, panelW, 40,
            Color(70, 82, 130, alpha/2));
        curY += 40;
    }

    int hItemH = 78;
    for (int i = 0; i < (int)history.size(); i++) {
        int hy = curY + i * (hItemH + 6);
        if (hy + hItemH > panelY + panelH - 6) break;

        const auto& n = history[i];
        bool hov = (mouseX >= panelX + 8 && mouseX < panelX + panelW - 8 &&
                    mouseY >= hy && mouseY < hy + hItemH);

        r.drawRoundedVerticalGradient(panelX + 8, hy, panelW - 16, hItemH, 10,
            hov ? Color(25, 32, 65, alpha) : Color(16, 20, 44, alpha*3/4),
            hov ? Color(18, 24, 52, alpha) : Color(12, 16, 38, alpha*3/4));
        r.drawRoundedRect(panelX + 8, hy, panelW - 16, hItemH, 10,
            Color(n.iconColor.r, n.iconColor.g, n.iconColor.b, n.read ? alpha/8 : alpha/4));
        r.drawRect(panelX + 8, hy, 3, hItemH,
            Color(n.iconColor.r, n.iconColor.g, n.iconColor.b, n.read ? alpha/2 : alpha));

        drawIcon(r, sf, panelX + 28, hy + hItemH/2, 12, n.category, n.iconColor);

        if (!n.read) {
            r.drawFilledCircle(panelX + panelW - 28, hy + 14, 4,
                Color(255, 100, 100, alpha));
        }

        // X clear button
        int xbx2 = panelX + panelW - 32;
        bool xhov2 = (mouseX >= xbx2 && mouseX < xbx2 + 20 &&
                      mouseY >= hy + 6 && mouseY < hy + 26);
        if (xhov2) r.drawFilledCircle(xbx2 + 9, hy + 14, 9, Color(255,80,80,alpha*3/4));
        r.drawLine(xbx2 + 5, hy + 9,  xbx2 + 13, hy + 19,
            Color(160, 172, 220, xhov2 ? alpha : alpha/3));
        r.drawLine(xbx2 + 13, hy + 9, xbx2 + 5,  hy + 19,
            Color(160, 172, 220, xhov2 ? alpha : alpha/3));

        if (font) r.drawText(font, n.title,
            panelX + 48, hy + 8,
            Color(n.read ? 160 : 210,
                  n.read ? 170 : 222,
                  n.read ? 200 : 255, alpha));
        if (sf) {
            std::string body = n.body;
            if ((int)body.size() > 36) body = body.substr(0, 34) + "…";
            r.drawText(sf, body,
                panelX + 48, hy + 26,
                Color(100, 115, 168, alpha));
            r.drawText(sf, n.appName,
                panelX + 48, hy + 44,
                Color(n.iconColor.r/2 + 60, n.iconColor.g/2 + 60, n.iconColor.b/2 + 60, alpha*3/4));
            r.drawTextRight(sf, n.timeStr,
                panelX + panelW - 36, hy + 44,
                Color(75, 90, 145, alpha));
        }
    }

    // ── "Do Not Disturb" toggle at bottom ─────────────────────
    int dndY = panelY + panelH - 42;
    r.drawLine(panelX + 8, dndY - 4, panelX + panelW - 8, dndY - 4,
        Color(40, 52, 95, alpha/2));
    r.drawRect(panelX, dndY, panelW, 42, Color(16, 20, 44, alpha));
    r.drawRoundedRect(panelX, dndY, panelW, 42, 16, Color(0,0,0,0));

    if (sf) r.drawText(sf, "Do Not Disturb",
        panelX + 16, dndY + 12,
        Color(145, 158, 205, alpha));

    // DND toggle (static off for now)
    int togX = panelX + panelW - 64, togY = dndY + 10, togW2 = 46, togH2 = 22;
    r.drawRoundedRect(togX, togY, togW2, togH2, togH2/2, Color(32, 40, 80, alpha));
    r.drawFilledCircle(togX + 4 + togH2/2, togY + togH2/2, togH2/2 - 2,
        Color(180, 190, 230, alpha * 3/4));
}

// ─── render ───────────────────────────────────────────────────────────────────

void NotificationManager::render(Renderer& r, TTF_Font* font, TTF_Font* smallFont) {
    // Count live
    int liveCount = 0;
    for (auto& n : notifications) if (!n.dismissed) liveCount++;

    // Render toasts bottom-right (above dock, stacked upward)
    int stackIdx = 0;
    for (auto& n : notifications) {
        if (n.dismissed && n.slideX >= 0.99f) continue;

        int posFromBottom = liveCount - stackIdx;
        int toastX = SCREEN_W - TOAST_W - 12;
        int toastY = SCREEN_H - DOCK_H - TOAST_PAD
                   - posFromBottom * (TOAST_H + TOAST_PAD);

        // Hover glow
        bool hov = (mouseX >= toastX && mouseX < toastX + TOAST_W &&
                    mouseY >= toastY && mouseY < toastY + TOAST_H);
        if (hov && !n.dismissed) {
            // Extend life on hover
            if (n.lifeTimer < 2.0f) n.lifeTimer = 2.0f;
        }

        renderToast(r, font, smallFont, n, toastX, toastY);
        stackIdx++;
    }

    // History panel
    renderHistoryPanel(r, font, smallFont);
}
