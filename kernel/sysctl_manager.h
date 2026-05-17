#ifndef SYSCTL_MANAGER_H
#define SYSCTL_MANAGER_H

#include <string>

class SysctlManager {
public:
    void loadDefaults();
    void apply(const std::string& key, const std::string& value);
    void applyPerformanceProfile();
    void applySecurityProfile();
};

#endif
