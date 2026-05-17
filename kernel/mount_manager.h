#ifndef MOUNT_MANAGER_H
#define MOUNT_MANAGER_H

#include <string>

class MountManager {
public:
    bool mountRoot(const std::string& device);
    bool mountProcfs();
    bool mountTmpfs();
    bool mountDevfs();
    bool mountHome();
};

#endif
