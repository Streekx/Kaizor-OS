#include "shadow_renderer.h"

void ShadowRenderer::render(
    Renderer& renderer,
    int x,
    int y,
    int width,
    int height
) {

    renderer.drawRoundedRect(
        x - 8,
        y - 8,
        width + 16,
        height + 16,
        22,
        Color(
            90,
            140,
            255,
            32
        )
    );

    renderer.drawRoundedRect(
        x - 4,
        y - 4,
        width + 8,
        height + 8,
        20,
        Color(
            0,
            0,
            0,
            50
        )
    );
}
