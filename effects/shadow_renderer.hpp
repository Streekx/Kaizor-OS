#pragma once

#include "../graphics/renderer.hpp"

class ShadowRenderer {

public:

    void renderShadow(
        Renderer& renderer,
        int x,
        int y,
        int width,
        int height
    );
};
