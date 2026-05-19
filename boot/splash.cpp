#include "splash.h"
#include <cmath>
#include <algorithm>

SplashScreen::SplashScreen(int w, int h)
    : screenW(w), screenH(h), frame(0), phase(SplashPhase::FADE_IN),
      fontLarge(nullptr), fontMedium(nullptr), fontSmall(nullptr),
      visibleLogs(0), fadeAlpha(255.0f), logoAlpha(0.0f),
      logoScale(0.5f), glowPulse(0.0f)
{
    logLines = {
        "[  0.000000] Kaizor kernel 1.0.0 booting on x86_64",
        "[  0.012418] Command line: BOOT_IMAGE=/vmlinuz root=/dev/nvme0n1p1 ro quiet",
        "[  0.031245] ACPI: 4 ACPI AML tables successfully loaded and verified",
        "[  0.088421] NR_CPUS: 8  nr_cpu_ids: 8  nr_node_ids: 1  NUMA: on",
        "[  0.099183] CPU0: Intel Core Ultra 9 285K @ 5.70GHz  Family: 6",
        "[  0.142311] Memory: 32768MB total  31842MB free  902MB reserved",
        "[  0.199834] PID hash table entries: 8192  (order: 4, 65536 bytes)",
        "[  0.231842] SLUB: HWalign=64  Order=0-3  MinObjects=0  CPUs=8",
        "[  0.312543] EXT4-fs: nvme0n1p1 mounted with ordered data mode",
        "[  0.398721] NET: Registered PF_INET6 protocol family  MTU: 1500",
        "[  0.421903] Mounting tmpfs on /dev/shm  /run  /run/lock",
        "[  0.512843] udev: Starting hotplug daemon  PID: 412",
        "[  0.589021] GPU: KaizorGFX accelerated renderer  VRAM: 24GB",
        "[  0.614237] KaizorWM: Display server protocol v4 initialized",
        "[  0.712843] Compositor: Wayland layer active  60Hz  1280x720",
        "[  0.801234] KaizorOS: All services ready.  Starting session.",
    };
}

void SplashScreen::setFonts(TTF_Font* large, TTF_Font* medium, TTF_Font* small) {
    fontLarge  = large;
    fontMedium = medium;
    fontSmall  = small;
}

void SplashScreen::updatePhase() {
    if      (frame < 50)  phase = SplashPhase::FADE_IN;
    else if (frame < 140) phase = SplashPhase::LOGO_APPEAR;
    else if (frame < 250) phase = SplashPhase::KERNEL_LOGS;
    else if (frame < 300) phase = SplashPhase::LOADING_DOTS;
    else if (frame < 340) phase = SplashPhase::FADE_OUT;
    else                  phase = SplashPhase::DONE;
}

void SplashScreen::update() {
    frame++;
    updatePhase();

    glowPulse = 0.7f + 0.3f * sinf(frame * 0.07f);

    if (phase == SplashPhase::FADE_IN) {
        float t = (float)frame / 50.0f;
        fadeAlpha = 255.0f * (1.0f - std::min(t, 1.0f));
    } else {
        fadeAlpha = 0.0f;
    }

    if (phase == SplashPhase::LOGO_APPEAR) {
        float t = (float)(frame - 50) / 90.0f;
        t = std::min(t, 1.0f);
        float ease = 1.0f - (1.0f - t) * (1.0f - t);
        logoAlpha = ease;
        logoScale = 0.55f + 0.45f * ease;
    } else if (phase >= SplashPhase::KERNEL_LOGS && phase < SplashPhase::FADE_OUT) {
        logoAlpha = 1.0f;
        logoScale = 1.0f;
    }

    if (phase == SplashPhase::KERNEL_LOGS) {
        int logFrame = frame - 140;
        visibleLogs  = logFrame / 7 + 1;
        if (visibleLogs > (int)logLines.size())
            visibleLogs = (int)logLines.size();
    }

    if (phase == SplashPhase::FADE_OUT) {
        float t = (float)(frame - 300) / 40.0f;
        fadeAlpha = 255.0f * std::min(t, 1.0f);
        logoAlpha = std::max(0.0f, 1.0f - std::min(t, 1.0f));
    }
}

void SplashScreen::renderBackground(Renderer& r) {
    r.drawVerticalGradient(0, 0, screenW, screenH,
        Color(4,  5, 14),
        Color(14, 6, 28));

    int cx = screenW / 2;
    int cy = screenH / 2;

    r.drawFilledCircle(cx, cy - 60, 380, Color(30,  10, 80, 14));
    r.drawFilledCircle(cx, cy - 60, 260, Color(40,  14, 110, 10));
    r.drawFilledCircle(cx - 200, cy + 200, 220, Color(10, 40, 90, 8));
    r.drawFilledCircle(cx + 220, cy + 160, 180, Color(50, 20, 90, 7));
}

void SplashScreen::renderGlowRings(Renderer& r, int cx, int cy, int baseR, float alpha) {
    for (int i = 7; i >= 1; i--) {
        int     rr  = baseR + i * 14;
        uint8_t ga  = (uint8_t)(alpha * 255.0f * 0.035f * glowPulse * (8 - i));
        r.drawFilledCircle(cx, cy, rr, Color(110, 60, 240, ga));
    }
}

void SplashScreen::renderKLetter(Renderer& r, int cx, int cy, int size, uint8_t alpha) {
    int hw = size / 2;
    Color kWhite(230, 240, 255, alpha);
    Color kDim(180, 190, 230, (uint8_t)(alpha * 0.7f));

    r.drawThickLine(cx - hw, cy - hw, cx - hw, cy + hw, 5, kWhite);
    r.drawThickLine(cx - hw + 1, cy,  cx + hw, cy - hw, 5, kWhite);
    r.drawThickLine(cx - hw + 1, cy,  cx + hw, cy + hw, 5, kWhite);

    r.drawThickLine(cx - hw - 1, cy - hw - 1, cx - hw - 1, cy + hw + 1, 2,
        Color(150, 120, 255, (uint8_t)(alpha * 0.4f)));
    (void)kDim;
}

void SplashScreen::renderLogo(Renderer& r) {
    if (logoAlpha < 0.01f) return;

    int cx    = screenW / 2;
    int cy    = screenH / 2 - 50;
    int baseR = (int)(64.0f * logoScale);

    uint8_t a = (uint8_t)(logoAlpha * 255.0f);

    renderGlowRings(r, cx, cy, baseR, logoAlpha * glowPulse);

    r.drawFilledCircle(cx, cy, baseR + 5,
        Color(70, 40, 160, (uint8_t)(a * 0.65f)));
    r.drawFilledCircle(cx, cy, baseR + 3,
        Color(90, 55, 200, (uint8_t)(a * 0.50f)));
    r.drawFilledCircle(cx, cy, baseR,
        Color(10,  7, 22, a));

    r.drawCircleOutline(cx, cy, baseR - 1,
        Color(140, 90, 255, (uint8_t)(a * 0.9f * glowPulse)));
    r.drawCircleOutline(cx, cy, baseR - 3,
        Color(100, 60, 200, (uint8_t)(a * 0.4f)));

    renderKLetter(r, cx, cy, (int)(baseR * 0.78f), a);

    if (fontLarge && logoAlpha > 0.25f) {
        uint8_t ta = (uint8_t)(std::min((logoAlpha - 0.25f) / 0.75f, 1.0f) * 255.0f);
        r.drawTextCentered(fontLarge, "KAIZOR  OS",
            cx - 220, cy + baseR + 22, 440, 44,
            Color(210, 220, 255, ta));
    }

    if (fontSmall && logoAlpha > 0.5f) {
        uint8_t va = (uint8_t)(std::min((logoAlpha - 0.5f) / 0.5f, 1.0f) * 160.0f);
        r.drawTextCentered(fontSmall, "Desktop Environment  v1.0.0",
            cx - 200, cy + baseR + 70, 400, 22,
            Color(120, 130, 180, va));
    }
}

void SplashScreen::renderKernelLogs(Renderer& r) {
    if (!fontSmall || visibleLogs <= 0) return;

    int totalLines  = (int)logLines.size();
    int lineH       = 17;
    int startY      = screenH - 20 - totalLines * lineH;
    int x           = 36;

    for (int i = 0; i < visibleLogs && i < totalLines; i++) {
        uint8_t alpha = 160;
        if (i == visibleLogs - 1) {
            int sub = (frame - 140) % 7;
            alpha   = (uint8_t)(160.0f * sub / 7.0f);
        }

        const std::string& line = logLines[i];
        size_t brEnd = line.find(']');
        if (brEnd != std::string::npos) {
            std::string bracket = line.substr(0, brEnd + 1);
            std::string rest    = line.substr(brEnd + 1);
            int bw = r.measureTextW(fontSmall, bracket);
            r.drawText(fontSmall, bracket, x,      startY + i * lineH, Color(70, 160, 255, alpha));
            r.drawText(fontSmall, rest,    x + bw, startY + i * lineH, Color(170, 180, 210, alpha));
        } else {
            r.drawText(fontSmall, line, x, startY + i * lineH, Color(170, 180, 210, alpha));
        }
    }
}

void SplashScreen::renderLoadingDots(Renderer& r) {
    int cx       = screenW / 2;
    int cy       = screenH / 2 + 130;
    int dotFrame = frame - 250;

    if (fontMedium) {
        r.drawTextCentered(fontMedium, "Starting session ...",
            cx - 160, cy - 38, 320, 28,
            Color(150, 160, 210, 200));
    }

    for (int i = 0; i < 3; i++) {
        int   off   = dotFrame - i * 10;
        if (off < 0) off = 0;
        float pulse = 0.35f + 0.65f * (0.5f + 0.5f * sinf(off * 0.20f));
        uint8_t alph = (uint8_t)(220.0f * pulse);
        int     dotR = (int)(9.0f * pulse);
        int     dotX = cx + (i - 1) * 34;
        r.drawFilledCircle(dotX, cy, dotR + 2,
            Color(100, 60, 200, (uint8_t)(alph * 0.4f)));
        r.drawFilledCircle(dotX, cy, dotR,
            Color(150, 100, 255, alph));
    }
}

void SplashScreen::renderFadeOverlay(Renderer& r) {
    if (fadeAlpha > 1.0f) {
        int fa = (int)std::min(fadeAlpha, 255.0f);
        r.drawRect(0, 0, screenW, screenH, Color(0, 0, 0, fa));
    }
}

void SplashScreen::render(Renderer& r) {
    renderBackground(r);

    if (phase >= SplashPhase::LOGO_APPEAR && phase <= SplashPhase::FADE_OUT) {
        renderLogo(r);
    }

    if (phase == SplashPhase::KERNEL_LOGS || phase == SplashPhase::LOADING_DOTS) {
        renderKernelLogs(r);
    }

    if (phase == SplashPhase::LOADING_DOTS) {
        renderLoadingDots(r);
    }

    renderFadeOverlay(r);
}

bool SplashScreen::finished() const {
    return phase == SplashPhase::DONE;
}
