#ifndef SDL_BACKEND_H
#define SDL_BACKEND_H

class SDLBackend {

public:

    bool init();

    void createWindow();

    void render();

    void eventLoop();

    void shutdown();
};

#endif
