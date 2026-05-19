#include "login_manager.h"
#include <cmath>
#include <ctime>
#include <algorithm>
#include <sstream>
#include <iomanip>

LoginManager::LoginManager(int w, int h)
    : screenW(w), screenH(h),
      phase(LoginPhase::FADE_IN), frame(0),
      fadeAlpha(255.0f), glowPulse(0.0f), authTimer(0.0f), cardSlide(80.0f),
      fontLarge(nullptr), fontMedium(nullptr), fontSmall(nullptr),
      showPowerMenu(false), mouseX(0), mouseY(0),
      loginBtnHover(false), powerBtnHover(false)
{
    cardW = 440;
    cardH = 520;
    cardX = (screenW - cardW) / 2;
    cardY = (screenH - cardH) / 2 - 20;

    pwFieldW = 320;
    pwFieldH = 48;
    pwFieldX = cardX + (cardW - pwFieldW) / 2;
    pwFieldY = cardY + 310;

    loginBtnW = 320;
    loginBtnH = 50;
    loginBtnX = cardX + (cardW - loginBtnW) / 2;
    loginBtnY = pwFieldY + pwFieldH + 18;

    powerBtnX = screenW - 52;
    powerBtnY = screenH - 52;
    powerBtnR = 20;

    SDL_StartTextInput();
}

void LoginManager::setFonts(TTF_Font* large, TTF_Font* medium, TTF_Font* small) {
    fontLarge  = large;
    fontMedium = medium;
    fontSmall  = small;
}

bool LoginManager::hitLoginBtn(int mx, int my) const {
    return mx >= loginBtnX && mx <= loginBtnX + loginBtnW &&
           my >= loginBtnY && my <= loginBtnY + loginBtnH;
}

bool LoginManager::hitPowerBtn(int mx, int my) const {
    int dx = mx - powerBtnX, dy = my - powerBtnY;
    return (dx * dx + dy * dy) <= (powerBtnR * powerBtnR);
}

void LoginManager::attemptLogin() {
    if (phase == LoginPhase::IDLE || phase == LoginPhase::FADE_IN) {
        phase     = LoginPhase::AUTHENTICATING;
        authTimer = 0.0f;
    }
}

void LoginManager::handleEvent(SDL_Event& e) {
    if (phase == LoginPhase::FADE_OUT || phase == LoginPhase::DONE) return;

    if (e.type == SDL_MOUSEMOTION) {
        mouseX = e.motion.x;
        mouseY = e.motion.y;
        loginBtnHover = hitLoginBtn(mouseX, mouseY);
        powerBtnHover = hitPowerBtn(mouseX, mouseY);
    }

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        if (hitLoginBtn(mouseX, mouseY)) {
            attemptLogin();
        } else if (hitPowerBtn(mouseX, mouseY)) {
            showPowerMenu = !showPowerMenu;
        }
    }

    if (e.type == SDL_KEYDOWN) {
        SDL_Keycode k = e.key.keysym.sym;
        if (k == SDLK_RETURN || k == SDLK_KP_ENTER) {
            attemptLogin();
        } else if (k == SDLK_BACKSPACE && !password.empty()) {
            password.pop_back();
        } else if (k == SDLK_ESCAPE) {
            showPowerMenu = false;
            password.clear();
        }
        if (phase == LoginPhase::IDLE || phase == LoginPhase::FADE_IN)
            phase = LoginPhase::IDLE;
    }

    if (e.type == SDL_TEXTINPUT) {
        if (phase == LoginPhase::IDLE || phase == LoginPhase::FADE_IN) {
            if (password.size() < 32) {
                password += e.text.text;
            }
            phase = LoginPhase::IDLE;
        }
    }
}

void LoginManager::update(float dt) {
    frame++;
    glowPulse = 0.65f + 0.35f * sinf(frame * 0.05f);

    if (phase == LoginPhase::FADE_IN) {
        fadeAlpha  = std::max(0.0f, fadeAlpha - 6.5f);
        cardSlide  = std::max(0.0f, cardSlide  - 4.0f);
        if (fadeAlpha <= 0.0f) {
            phase     = LoginPhase::IDLE;
            cardSlide = 0.0f;
        }
    }

    if (phase == LoginPhase::AUTHENTICATING) {
        authTimer += dt;
        if (authTimer >= 1.4f) {
            phase = LoginPhase::SUCCESS;
        }
    }

    if (phase == LoginPhase::SUCCESS) {
        phase = LoginPhase::FADE_OUT;
    }

    if (phase == LoginPhase::FADE_OUT) {
        fadeAlpha = std::min(255.0f, fadeAlpha + 7.0f);
        if (fadeAlpha >= 255.0f) {
            phase = LoginPhase::DONE;
            SDL_StopTextInput();
        }
    }
}

static std::string getTimeString() {
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << t->tm_hour
        << ":" << std::setw(2) << std::setfill('0') << t->tm_min;
    return oss.str();
}

static std::string getDateString() {
    static const char* days[]   = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
    static const char* months[] = {"January","February","March","April","May","June",
                                   "July","August","September","October","November","December"};
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    std::ostringstream oss;
    oss << days[t->tm_wday] << ",  " << months[t->tm_mon] << "  " << t->tm_mday;
    return oss.str();
}

void LoginManager::renderBackground(Renderer& r) {
    r.drawVerticalGradient(0, 0, screenW, screenH,
        Color(6,  8, 22),
        Color(18, 10, 36));

    r.drawFilledCircle(screenW / 2, screenH / 2,      400, Color(40,  15, 100, 20));
    r.drawFilledCircle(160,         screenH - 180,     280, Color(20,  50, 120, 15));
    r.drawFilledCircle(screenW - 200, 180,             220, Color(60,  20, 110, 13));
    r.drawFilledCircle(screenW / 2 + 100, screenH / 2, 300, Color(10,  30,  90, 12));

    r.drawRect(0, 0, screenW, screenH, Color(4, 5, 16, 100));
}

void LoginManager::renderLoginCard(Renderer& r) {
    int slideY = (int)cardSlide;

    int cx  = cardX;
    int cy  = cardY + slideY;
    int cw  = cardW;
    int ch  = cardH;

    r.drawFilledCircle(cx + cw / 2, cy + ch / 2, std::max(cw, ch),
        Color(60, 30, 150, (uint8_t)(12 * glowPulse)));
    r.drawFilledCircle(cx + cw / 2, cy + ch / 2, (int)(std::max(cw, ch) * 0.7f),
        Color(70, 35, 160, (uint8_t)(8 * glowPulse)));

    r.drawRoundedRect(cx - 2, cy - 2, cw + 4, ch + 4, 22,
        Color(80, 50, 180, (uint8_t)(60 * glowPulse)));
    r.drawRoundedVerticalGradient(cx, cy, cw, ch, 20,
        Color(22, 18, 46, 230),
        Color(16, 12, 34, 240));

    r.drawRoundedOutline(cx, cy, cw, ch, 20,
        Color(100, 70, 220, (uint8_t)(80 * glowPulse)), 2);

    r.drawRoundedRect(cx + 20, cy + 1, cw - 40, 28, 14,
        Color(255, 255, 255, 6));

    int avatarCX = cx + cw / 2;
    int avatarCY = cy + 86;
    renderAvatar(r, avatarCX, avatarCY + slideY - slideY, avatarCY);

    int usernameY = cy + 156;
    if (fontLarge) {
        r.drawTextCentered(fontLarge, "Kaizor",
            cx, usernameY, cw, 36,
            Color(220, 228, 255, 240));
    }

    if (fontSmall) {
        r.drawTextCentered(fontSmall, "Desktop User",
            cx, usernameY + 38, cw, 22,
            Color(130, 140, 190, 180));
    }

    renderPasswordField(r);
    renderLoginButton(r);
}

void LoginManager::renderAvatar(Renderer& r, int cx, int /*cy*/, int realCY) {
    int radius = 52;

    for (int i = 5; i >= 1; i--) {
        r.drawFilledCircle(cx, realCY,
            radius + i * 7,
            Color(90, 55, 200, (uint8_t)(18 * glowPulse * (6 - i))));
    }

    r.drawFilledCircle(cx, realCY, radius + 3, Color(70, 40, 170, 200));
    r.drawFilledCircle(cx, realCY, radius,     Color(10,  7, 22, 255));

    r.drawVerticalGradient(cx - radius, realCY - radius,
        radius * 2, radius * 2,
        Color(80, 50, 180, 220),
        Color(40, 20, 100, 200));
    r.drawFilledCircle(cx, realCY, radius - 1, Color(0, 0, 0, 0));

    if (fontLarge) {
        r.drawTextCentered(fontLarge, "K",
            cx - radius, realCY - radius / 2,
            radius * 2, radius,
            Color(230, 240, 255, 240));
    }

    r.drawCircleOutline(cx, realCY, radius,
        Color(140, 100, 255, (uint8_t)(180 * glowPulse)));
}

void LoginManager::renderPasswordField(Renderer& r) {
    int slideY = (int)cardSlide;
    int fx = pwFieldX;
    int fy = pwFieldY + slideY;
    int fw = pwFieldW;
    int fh = pwFieldH;

    bool focused = (phase == LoginPhase::IDLE || phase == LoginPhase::AUTHENTICATING);
    bool typing  = !password.empty();

    Color borderCol = focused
        ? Color(110, 75, 240, (uint8_t)(160 + 80 * glowPulse))
        : Color(60, 55, 100, 140);

    r.drawRoundedRect(fx - 2, fy - 2, fw + 4, fh + 4, 14,
        Color(borderCol.r, borderCol.g, borderCol.b, borderCol.a));
    r.drawRoundedRect(fx, fy, fw, fh, 12, Color(14, 12, 30, 230));

    if (!typing && fontSmall) {
        r.drawText(fontSmall, "Enter password ...",
            fx + 16, fy + (fh - 16) / 2,
            Color(80, 85, 130, 160));
    }

    int dotX = fx + 16;
    int dotY = fy + fh / 2;
    int shown = std::min((int)password.size(), 20);
    for (int i = 0; i < shown; i++) {
        r.drawFilledCircle(dotX + i * 18, dotY, 5, Color(200, 210, 255, 230));
    }

    if (focused && (frame / 30) % 2 == 0) {
        int cursorX = fx + 16 + shown * 18;
        r.drawRect(cursorX + 2, fy + 10, 2, fh - 20, Color(160, 140, 255, 200));
    }
}

void LoginManager::renderLoginButton(Renderer& r) {
    int slideY = (int)cardSlide;
    int bx = loginBtnX;
    int by = loginBtnY + slideY;
    int bw = loginBtnW;
    int bh = loginBtnH;

    bool authenticating = (phase == LoginPhase::AUTHENTICATING);

    Color topCol = loginBtnHover
        ? Color(110, 70, 240, 255)
        : Color(85,  50, 210, 255);
    Color botCol = loginBtnHover
        ? Color(75,  40, 200, 255)
        : Color(55,  30, 175, 255);

    if (authenticating) {
        topCol = Color(60, 180, 120, 255);
        botCol = Color(40, 140,  90, 255);
    }

    r.drawFilledCircle(bx + bw / 2, by + bh / 2, bw / 2 + 4,
        Color(topCol.r, topCol.g, topCol.b, (uint8_t)(30 * glowPulse)));

    r.drawRoundedVerticalGradient(bx, by, bw, bh, 14, topCol, botCol);
    r.drawRoundedOutline(bx, by, bw, bh, 14,
        Color(200, 180, 255, 60), 1);

    if (fontMedium) {
        const char* label = authenticating ? "Authenticating ..." : "Log In";
        r.drawTextCentered(fontMedium, label,
            bx, by, bw, bh,
            Color(240, 244, 255, 255));
    }
}

void LoginManager::renderPowerButton(Renderer& r) {
    Color bg  = powerBtnHover
        ? Color(200, 60, 60, 200)
        : Color(40, 36, 70, 180);
    Color brd = powerBtnHover
        ? Color(240, 80, 80, 200)
        : Color(90, 80, 140, 140);

    r.drawFilledCircle(powerBtnX, powerBtnY, powerBtnR + 3,
        Color(bg.r, bg.g, bg.b, 30));
    r.drawFilledCircle(powerBtnX, powerBtnY, powerBtnR, bg);
    r.drawCircleOutline(powerBtnX, powerBtnY, powerBtnR, brd);

    int s = 8;
    r.drawThickLine(powerBtnX, powerBtnY - s, powerBtnX, powerBtnY + s, 3,
        Color(220, 220, 255, 200));
    r.drawThickLine(powerBtnX - s, powerBtnY, powerBtnX + s, powerBtnY, 3,
        Color(220, 220, 255, 200));
}

void LoginManager::renderPowerMenu(Renderer& r) {
    if (!showPowerMenu) return;
    int mw = 200, mh = 120, mx2 = powerBtnX - mw - 10, my2 = powerBtnY - mh;
    r.drawRoundedRect(mx2, my2, mw, mh, 14, Color(22, 18, 44, 240));
    r.drawRoundedOutline(mx2, my2, mw, mh, 14, Color(80, 60, 160, 140), 1);

    struct { const char* label; Color col; } items[] = {
        {"  Restart",  Color(220, 200, 255, 220)},
        {"  Shutdown", Color(255, 120, 120, 220)},
    };
    int itemH = 50;
    for (int i = 0; i < 2; i++) {
        int iy = my2 + i * itemH + 10;
        if (mouseY >= iy && mouseY <= iy + itemH - 4 &&
            mouseX >= mx2 && mouseX <= mx2 + mw) {
            r.drawRoundedRect(mx2 + 4, iy, mw - 8, itemH - 4, 10,
                Color(60, 45, 120, 100));
        }
        if (fontMedium)
            r.drawText(fontMedium, items[i].label, mx2 + 14, iy + (itemH - 20) / 2,
                items[i].col);
    }
}

void LoginManager::renderClock(Renderer& r) {
    std::string timeStr = getTimeString();
    std::string dateStr = getDateString();

    int clockX = screenW / 2;
    int clockY = 60;

    if (fontLarge) {
        r.drawTextCentered(fontLarge, timeStr,
            clockX - 120, clockY, 240, 44,
            Color(230, 235, 255, 230));
    }
    if (fontSmall) {
        r.drawTextCentered(fontSmall, dateStr,
            clockX - 160, clockY + 46, 320, 22,
            Color(150, 158, 200, 180));
    }
}

void LoginManager::renderFadeOverlay(Renderer& r) {
    if (fadeAlpha > 1.0f) {
        int fa = (int)std::min(fadeAlpha, 255.0f);
        r.drawRect(0, 0, screenW, screenH, Color(0, 0, 0, fa));
    }
}

void LoginManager::render(Renderer& r) {
    renderBackground(r);
    renderClock(r);
    renderLoginCard(r);
    renderPowerButton(r);
    renderPowerMenu(r);
    renderFadeOverlay(r);
}

bool LoginManager::isComplete() const {
    return phase == LoginPhase::DONE;
}

void LoginManager::setMousePos(int mx, int my) {
    mouseX = mx;
    mouseY = my;
    loginBtnHover = hitLoginBtn(mx, my);
    powerBtnHover = hitPowerBtn(mx, my);
}
