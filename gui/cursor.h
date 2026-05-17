#ifndef CURSOR_H
#define CURSOR_H

class Cursor {

public:

    int x;
    int y;

    Cursor();

    void move(
        int newX,
        int newY
    );

    void render();
};

#endif
