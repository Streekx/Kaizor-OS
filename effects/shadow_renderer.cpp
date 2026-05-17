#include "shadow_renderer.hpp"

void ShadowRenderer::renderShadow(
    Renderer& renderer,
    int x,
    int y,
    int width,
    int height
) {

    renderer.drawRect(
        x + 12,
        y + 12,
        width,
        height,
        Color(
            0,
            0,
            0,
            90
        )
    );
}
