#ifndef RC_SERVICE_H
#define RC_SERVICE_H

#include <string>

class RCService {
public:
    bool start(const std::string& name);
    bool stop(const std::string& name);
    bool enable(const std::string& name);
};

#endif
