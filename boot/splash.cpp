#include "splash.h"

SplashScreen::SplashScreen() {

    frame = 0;
}

bool SplashScreen::finished() {

    return frame > 180;
}

void SplashScreen::update() {

    frame++;
}

void SplashScreen::render(
    Renderer& renderer
) {

    renderer.clear(
        Color(
            4,
            6,
            14
        )
    );

    /* CENTER LOGO */

    renderer.drawRoundedRect(
        540,
        240,
        200,
        200,
        40,
        Color(
            70,
            120,
            255,
            255
        )
    );

    renderer.drawRoundedRect(
        585,
        285,
        110,
        110,
        28,
        Color(
            220,
            240,
            255,
            220
        )
    );

    /* LOADING BAR */

    renderer.drawRoundedRect(
        420,
        520,
        440,
        14,
        8,
        Color(
            40,
            44,
            62,
            255
        )
    );

    int progress = frame * 2;

    if (progress > 440) {
        progress = 440;
    }

    renderer.drawRoundedRect(
        420,
        520,
        progress,
        14,
        8,
        Color(
            90,
            180,
            255,
            255
        )
    );
}
