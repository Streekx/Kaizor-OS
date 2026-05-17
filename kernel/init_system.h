#ifndef INIT_SYSTEM_H
#define INIT_SYSTEM_H

class InitSystem {
public:
    void loadServices();
    void startCoreServices();
    void startNetworkServices();
    void startAudioServices();
    void startDesktopServices();
};

#endif
