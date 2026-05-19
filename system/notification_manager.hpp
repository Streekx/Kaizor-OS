#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <string>
#include "../graphics/renderer.hpp"

enum class NotifCategory {
    SYSTEM,
    APP,
    MESSAGE,
    ALERT,
    UPDATE,
    DOWNLOAD
};

struct Notification {
    int         id;
    std::string appName;
    std::string title;
    std::string body;
    std::string timeStr;
    NotifCategory category;
    Color       iconColor;
    bool        read;
    bool        dismissed;

    // Animation state
    float       slideX;     // 0 = fully in, 1 = fully out to right
    float       lifeTimer;  // seconds remaining before auto-dismiss
    float       opacity;
    bool        isPinned;   // stays until manually dismissed
};

class NotificationManager {
private:
    std::vector<Notification> notifications;
    std::vector<Notification> history;   // dismissed/expired ones

    bool  historyOpen;
    float historySlide;   // 0=closed, 1=open
    int   nextId;
    int   mouseX, mouseY;
    float frameTime;

    static const int  TOAST_W      = 340;
    static const int  TOAST_H      = 88;
    static const int  TOAST_PAD    = 10;
    static const int  MAX_VISIBLE  = 4;
    static const int  SCREEN_W     = 1280;
    static const int  SCREEN_H     = 720;
    static const int  DOCK_H       = 94;
    static const int  TASKBAR_H    = 46;

    void renderToast(Renderer& r, TTF_Font* font, TTF_Font* sf,
                     const Notification& n, int x, int y) const;
    void renderHistoryPanel(Renderer& r, TTF_Font* font, TTF_Font* sf);

public:
    NotificationManager();

    // Push a new notification (auto-generates time string)
    void push(const std::string& appName,
              const std::string& title,
              const std::string& body,
              NotifCategory cat  = NotifCategory::APP,
              Color iconColor    = Color(80,140,255),
              bool  pinned       = false);

    // Seed demo notifications on first start
    void seedDemoNotifications();

    void handleEvent(SDL_Event& e);
    void update(float dt);
    void render(Renderer& r, TTF_Font* font, TTF_Font* smallFont);

    void toggleHistory();
    bool isHistoryOpen() const { return historyOpen; }
    int  unreadCount()   const;
    int  activeCount()   const;

    void setMousePos(int mx, int my) { mouseX = mx; mouseY = my; }
};
