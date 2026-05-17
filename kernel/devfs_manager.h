#ifndef DEVFS_MANAGER_H
#define DEVFS_MANAGER_H

class DevFSManager {
public:
    void init();
    void applyRules();
    void allowGPU();
    void allowInput();
};

#endif
