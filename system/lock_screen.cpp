#include "lock_screen.h"
#include <cmath>
#include <ctime>
#include <algorithm>
#include <sstream>
#include <iomanip>

LockScreen::LockScreen(int w, int h)
    : screenW(w), screenH(h),
      phase(LockPhase::WAKE_ANIMATION), frame(0),
      glowPulse(0.0f), wakeAlpha(255.0f),
      fontXL(nullptr), fontLarge(nullptr), fontMedium(nullptr), fontSmall(nullptr),
      mouseX(0), mouseY(0), unlocked(false), pwFieldHover(false)
{
    pwFieldW = 340;
    pwFieldH = 52;
    pwFieldX = (screenW - pwFieldW) / 2;
    pwFieldY = screenH / 2 + 60;
}

void LockScreen::setFonts(TTF_Font* xl, TTF_Font* large, TTF_Font* medium, TTF_Font* small) {
    fontXL     = xl;
    fontLarge  = large;
    fontMedium = medium;
    fontSmall  = small;
}

void LockScreen::lock() {
    phase     = LockPhase::WAKE_ANIMATION;
    wakeAlpha = 255.0f;
    frame     = 0;
    password.clear();
    unlocked  = false;
    SDL_StartTextInput();
}

void LockScreen::handleEvent(SDL_Event& e) {
    if (unlocked) return;

    if (e.type == SDL_MOUSEMOTION) {
        mouseX = e.motion.x;
        mouseY = e.motion.y;
        pwFieldHover = (mouseX >= pwFieldX && mouseX <= pwFieldX + pwFieldW &&
                        mouseY >= pwFieldY && mouseY <= pwFieldY + pwFieldH);
    }

    if (e.type == SDL_KEYDOWN) {
        SDL_Keycode k = e.key.keysym.sym;
        if (k == SDLK_RETURN || k == SDLK_KP_ENTER) {
            phase    = LockPhase::UNLOCKING;
            unlocked = true;
            SDL_StopTextInput();
        } else if (k == SDLK_BACKSPACE && !password.empty()) {
            password.pop_back();
        } else if (k == SDLK_ESCAPE) {
            password.clear();
        }
        if (phase == LockPhase::IDLE)
            phase = LockPhase::TYPING;
    }

    if (e.type == SDL_TEXTINPUT) {
        if (password.size() < 32)
            password += e.text.text;
        if (phase == LockPhase::IDLE || phase == LockPhase::WAKE_ANIMATION)
            phase = LockPhase::TYPING;
    }
}

void LockScreen::update(float dt) {
    (void)dt;
    frame++;
    glowPulse = 0.65f + 0.35f * sinf(frame * 0.05f);

    if (phase == LockPhase::WAKE_ANIMATION) {
        wakeAlpha = std::max(0.0f, wakeAlpha - 5.5f);
        if (wakeAlpha <= 0.0f) {
            phase = LockPhase::IDLE;
            SDL_StartTextInput();
        }
    }
}

static std::string lockTimeString() {
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << t->tm_hour
        << ":" << std::setw(2) << std::setfill('0') << t->tm_min;
    return oss.str();
}

static std::string lockDateString() {
    static const char* days[]   = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
    static const char* months[] = {"January","February","March","April","May","June",
                                   "July","August","September","October","November","December"};
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    std::ostringstream oss;
    oss << days[t->tm_wday] << ",  " << months[t->tm_mon] << "  " << t->tm_mday;
    return oss.str();
}

void LockScreen::renderBackground(Renderer& r) {
    r.drawVerticalGradient(0, 0, screenW, screenH,
        Color(4,  6, 16),
        Color(14,  8, 30));

    int cx = screenW / 2, cy = screenH / 2;
    r.drawFilledCircle(cx, cy - 80, 420, Color(40, 15, 100, 18));
    r.drawFilledCircle(cx, cy - 80, 280, Color(55, 20, 130, 12));
    r.drawFilledCircle(180, screenH - 160, 260, Color(20, 50, 120, 14));
    r.drawFilledCircle(screenW - 180, 180,  200, Color(60, 25, 110, 11));

    r.drawRect(0, 0, screenW, screenH, Color(0, 0, 0, 80));
}

void LockScreen::renderClock(Renderer& r) {
    std::string ts = lockTimeString();
    std::string ds = lockDateString();

    int cx = screenW / 2;
    int ty = screenH / 2 - 180;

    if (fontXL) {
        int tw = r.measureTextW(fontXL, ts);
        r.drawTextShadow(fontXL, ts,
            cx - tw / 2, ty,
            Color(235, 240, 255, 240),
            Color(0, 0, 0, 80), 2, 3);
    }

    if (fontMedium) {
        r.drawTextCentered(fontMedium, ds,
            cx - 220, ty + 72, 440, 30,
            Color(160, 168, 210, 200));
    }
}

void LockScreen::renderPasswordField(Renderer& r) {
    int fx = pwFieldX, fy = pwFieldY;
    int fw = pwFieldW, fh = pwFieldH;
    bool active = (phase == LockPhase::TYPING || phase == LockPhase::IDLE);

    Color borderCol = (active || pwFieldHover)
        ? Color(110, 75, 240, (uint8_t)(160 + 80 * glowPulse))
        : Color(60, 55, 100, 120);

    r.drawRoundedRect(fx - 2, fy - 2, fw + 4, fh + 4, 15,
        Color(borderCol.r, borderCol.g, borderCol.b, borderCol.a));
    r.drawRoundedRect(fx, fy, fw, fh, 13,
        Color(14, 12, 28, 220));

    int shown = std::min((int)password.size(), 22);
    int dotY  = fy + fh / 2;
    int startDotX = fx + fw / 2 - shown * 9;

    for (int i = 0; i < shown; i++) {
        r.drawFilledCircle(startDotX + i * 18, dotY, 5,
            Color(190, 200, 255, 230));
    }

    if (password.empty() && fontSmall) {
        r.drawTextCentered(fontSmall, "Enter password to unlock",
            fx, fy, fw, fh,
            Color(80, 85, 130, 150));
    }

    if (active && (frame / 30) % 2 == 0) {
        int cursorX = startDotX + shown * 18 + (shown == 0 ? fw / 2 - 2 : 2);
        r.drawRect(cursorX, fy + 10, 2, fh - 20, Color(160, 140, 255, 200));
    }
}

void LockScreen::renderHint(Renderer& r) {
    if (!fontSmall) return;

    r.drawTextCentered(fontSmall,
        "Press  Enter  to unlock  |  L  to wake",
        (screenW - 400) / 2, pwFieldY + pwFieldH + 18,
        400, 22,
        Color(80, 86, 130, 140));
}

void LockScreen::renderUnlockCard(Renderer& r) {
    int cardW = 420, cardH = 200;
    int cardX = (screenW - cardW) / 2;
    int cardY = screenH / 2 - 10;

    r.drawRoundedRect(cardX - 2, cardY - 2, cardW + 4, cardH + 4, 22,
        Color(80, 50, 180, (uint8_t)(50 * glowPulse)));
    r.drawRoundedVerticalGradient(cardX, cardY, cardW, cardH, 20,
        Color(20, 16, 42, 220),
        Color(14, 10, 30, 230));
    r.drawRoundedOutline(cardX, cardY, cardW, cardH, 20,
        Color(90, 60, 200, (uint8_t)(70 * glowPulse)), 1);
    r.drawRoundedRect(cardX + 18, cardY + 1, cardW - 36, 22, 12,
        Color(255, 255, 255, 5));

    if (fontSmall) {
        r.drawTextCentered(fontSmall, "LOCKED",
            cardX, cardY + 14, cardW, 22,
            Color(120, 130, 180, 180));
    }
}

void LockScreen::renderWakeOverlay(Renderer& r) {
    if (wakeAlpha > 1.0f) {
        int fa = (int)std::min(wakeAlpha, 255.0f);
        r.drawRect(0, 0, screenW, screenH, Color(0, 0, 0, fa));
    }
}

void LockScreen::render(Renderer& r) {
    renderBackground(r);
    renderClock(r);
    renderUnlockCard(r);
    renderPasswordField(r);
    renderHint(r);
    renderWakeOverlay(r);
}

bool LockScreen::isUnlocked() const {
    return unlocked;
}

void LockScreen::setMousePos(int mx, int my) {
    mouseX = mx;
    mouseY = my;
    pwFieldHover = (mx >= pwFieldX && mx <= pwFieldX + pwFieldW &&
                    my >= pwFieldY && my <= pwFieldY + pwFieldH);
}
