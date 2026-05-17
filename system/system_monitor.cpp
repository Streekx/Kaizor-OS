#include <iostream>

#include "system_monitor.h"

using namespace std;

void SystemMonitor::cpuUsage() {

    cout << "[MONITOR] CPU 24%"
         << endl;
}

void SystemMonitor::ramUsage() {

    cout << "[MONITOR] RAM 3.2GB"
         << endl;
}

void SystemMonitor::gpuUsage() {

    cout << "[MONITOR] GPU 41%"
         << endl;
}
