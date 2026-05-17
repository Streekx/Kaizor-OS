#pragma once

#include "../graphics/renderer.hpp"

class BlurEngine {

public:

    void renderBlurLayer(
        Renderer& renderer,
        int x,
        int y,
        int width,
        int height
    );
};
