#include "session_manager.h"
#include <cmath>
#include <algorithm>
#include <iostream>

SessionManager::SessionManager(int w, int h)
    : screenW(w), screenH(h),
      phase(SessionPhase::FADE_IN), frame(0),
      fadeAlpha(255.0f), globalProgress(0.0f),
      fontLarge(nullptr), fontMedium(nullptr), fontSmall(nullptr),
      currentService(0)
{
    services = {
        { "Compositor Engine",    0.0f, 0.024f, false },
        { "Display Server",       0.0f, 0.028f, false },
        { "Dock & Taskbar",       0.0f, 0.032f, false },
        { "Desktop Shell",        0.0f, 0.030f, false },
        { "Application Services", 0.0f, 0.022f, false },
    };
    std::cout << "[SESSION] Kaizor Desktop Session starting..." << std::endl;
}

void SessionManager::setFonts(TTF_Font* large, TTF_Font* medium, TTF_Font* small) {
    fontLarge  = large;
    fontMedium = medium;
    fontSmall  = small;
}

void SessionManager::update(float dt) {
    (void)dt;
    frame++;

    if (phase == SessionPhase::FADE_IN) {
        fadeAlpha = std::max(0.0f, fadeAlpha - 7.0f);
        if (fadeAlpha <= 0.0f)
            phase = SessionPhase::LOADING;
    }

    if (phase == SessionPhase::LOADING) {
        if (currentService < (int)services.size()) {
            auto& svc = services[currentService];
            svc.progress = std::min(1.0f, svc.progress + svc.speed);
            if (svc.progress >= 1.0f) {
                svc.complete = true;
                currentService++;
                std::cout << "[SESSION] Loaded: " << svc.name << std::endl;
            }
        } else {
            phase = SessionPhase::FADE_OUT;
        }

        int done = 0;
        for (auto& s : services) if (s.complete) done++;
        globalProgress = (float)done / (float)services.size();
    }

    if (phase == SessionPhase::FADE_OUT) {
        fadeAlpha = std::min(255.0f, fadeAlpha + 6.5f);
        if (fadeAlpha >= 255.0f) {
            phase = SessionPhase::DONE;
            std::cout << "[SESSION] Kaizor Desktop Session initialized." << std::endl;
        }
    }
}

void SessionManager::renderProgressBar(Renderer& r, int x, int y, int w, int h,
                                        float progress, const std::string& label,
                                        bool active) {
    r.drawRoundedRect(x, y, w, h, h / 2,
        Color(18, 14, 36, 200));
    r.drawRoundedOutline(x, y, w, h, h / 2,
        Color(60, 50, 110, 140), 1);

    if (progress > 0.0f) {
        int filled = (int)(w * std::min(progress, 1.0f));
        if (filled > h) {
            float pulse = active ? (0.8f + 0.2f * sinf(frame * 0.18f)) : 1.0f;
            Color barTop(
                (int)(80  + 60  * pulse),
                (int)(45  + 20  * pulse),
                (int)(220 - 20 * pulse),
                255);
            Color barBot(
                (int)(50  + 30  * pulse),
                (int)(30  + 15  * pulse),
                (int)(180 - 15 * pulse),
                255);
            r.drawRoundedVerticalGradient(x, y, filled, h, h / 2, barTop, barBot);

            if (active) {
                r.drawRoundedRect(x + filled - h, y, h, h, h / 2,
                    Color(200, 180, 255, (int)(80 * pulse)));
            }
        }
    }

    if (fontSmall) {
        Color labelCol = active
            ? Color(200, 210, 255, 220)
            : (progress >= 1.0f ? Color(100, 200, 140, 200) : Color(100, 105, 150, 160));
        r.drawText(fontSmall, label, x + 12, y - 18, labelCol);

        if (progress >= 1.0f && fontSmall) {
            r.drawTextRight(fontSmall, "OK", x + w - 8, y - 18,
                Color(80, 200, 120, 200));
        }
    }
}

void SessionManager::renderBackground(Renderer& r) {
    r.drawVerticalGradient(0, 0, screenW, screenH,
        Color(4, 5, 14),
        Color(12, 6, 24));

    int cx = screenW / 2, cy = screenH / 2;
    float pulse = 0.7f + 0.3f * sinf(frame * 0.04f);
    r.drawFilledCircle(cx, cy, 340, Color(35, 12, 90, (int)(14 * pulse)));
    r.drawFilledCircle(cx, cy, 220, Color(45, 16, 110, (int)(10 * pulse)));
}

void SessionManager::renderProgressBars(Renderer& r) {
    int barW  = 500;
    int barH  = 10;
    int startX = (screenW - barW) / 2;
    int startY = screenH / 2 + 20;
    int spacing = 46;

    for (int i = 0; i < (int)services.size(); i++) {
        bool active = (i == currentService);
        renderProgressBar(r,
            startX, startY + i * spacing,
            barW, barH,
            services[i].progress,
            services[i].name,
            active);
    }
}

void SessionManager::renderStatusText(Renderer& r) {
    int cx = screenW / 2;
    int cy = screenH / 2 - 120;

    if (fontLarge) {
        r.drawTextCentered(fontLarge, "KAIZOR  OS",
            cx - 220, cy, 440, 44,
            Color(200, 210, 255, 220));
    }

    if (fontMedium) {
        const char* label = (currentService < (int)services.size())
            ? "Initializing session ..."
            : "Session ready.";
        r.drawTextCentered(fontMedium, label,
            cx - 200, cy + 52, 400, 30,
            Color(130, 140, 190, 200));
    }

    if (fontSmall && currentService < (int)services.size()) {
        std::string loadingText = "Loading: " + services[currentService].name;
        r.drawTextCentered(fontSmall, loadingText,
            cx - 220, cy + 88, 440, 22,
            Color(90, 100, 150, 180));
    }
}

void SessionManager::renderFadeOverlay(Renderer& r) {
    if (fadeAlpha > 1.0f) {
        int fa = (int)std::min(fadeAlpha, 255.0f);
        r.drawRect(0, 0, screenW, screenH, Color(0, 0, 0, fa));
    }
}

void SessionManager::render(Renderer& r) {
    renderBackground(r);
    renderStatusText(r);
    renderProgressBars(r);
    renderFadeOverlay(r);
}

bool SessionManager::isComplete() const {
    return phase == SessionPhase::DONE;
}
