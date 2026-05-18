#pragma once

#include "../graphics/renderer.hpp"

class SplashScreen {

private:

    int frame;

public:

    SplashScreen();

    bool finished();

    void update();

    void render(
        Renderer& renderer
    );
};
