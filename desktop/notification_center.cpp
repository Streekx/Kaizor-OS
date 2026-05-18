#include "notification_center.h"

void NotificationCenter::render(
    Renderer& renderer
) {

    renderer.drawRoundedRect(
        980,
        70,
        260,
        420,
        24,
        Color(
            18,
            24,
            42,
            230
        )
    );

    int y = 110;

    for (
        int i = 0;
        i < 4;
        i++
    ) {

        renderer.drawRoundedRect(
            1005,
            y,
            210,
            70,
            18,
            Color(
                40,
                52,
                88,
                255
            )
        );

        y += 92;
    }
}
