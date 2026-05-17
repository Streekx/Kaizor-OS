#pragma once

struct Color {

    int r;
    int g;
    int b;
    int a;

    Color(
        int red,
        int green,
        int blue,
        int alpha = 255
    ) {

        r = red;
        g = green;
        b = blue;
        a = alpha;
    }
};
