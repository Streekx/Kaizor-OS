#include "taskbar.hpp"

#include "ui_theme.hpp"

void Taskbar::render(
    Renderer& renderer
) {

    renderer.drawRect(
        0,
        670,
        1280,
        50,
        UITheme::taskbar()
    );

    /* START BUTTON */

    renderer.drawRect(
        15,
        680,
        32,
        32,
        UITheme::accent()
    );
}
