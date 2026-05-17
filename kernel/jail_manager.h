#ifndef JAIL_MANAGER_H
#define JAIL_MANAGER_H

#include <string>

class JailManager {
public:
    void init();
    bool createJail(const std::string& name);
    bool startJail(const std::string& name);
};

#endif
