#ifndef FREEBSD_BRIDGE_H
#define FREEBSD_BRIDGE_H

class FreeBSDBridge {

public:

    void detectKernel();

    void mountSystem();

    void startUserspace();
};

#endif
