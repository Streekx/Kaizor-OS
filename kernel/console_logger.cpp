#include <iostream>
#include "console_logger.h"

using namespace std;

void ConsoleLogger::info(const string& msg) {
    cout << "[INFO] " << msg << endl;
}

void ConsoleLogger::warn(const string& msg) {
    cout << "[WARN] " << msg << endl;
}

void ConsoleLogger::error(const string& msg) {
    cout << "[ERROR] " << msg << endl;
}
