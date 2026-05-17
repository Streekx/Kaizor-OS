#include <iostream>

#include "app_launcher.h"

using namespace std;

void AppLauncher::launch(
    const char* app
) {

    cout << "[APP] Launching: "
         << app
         << endl;
}
