#pragma once

#include "../graphics/renderer.hpp"

class ShadowRenderer {

public:

    void render(
        Renderer& renderer,
        int x,
        int y,
        int width,
        int height
    );
};
