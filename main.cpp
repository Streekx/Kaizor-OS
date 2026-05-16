
#include <iostream>
using namespace std;

void systemInit() {
    cout << "[CORE] Init Done" << endl;
}

class DesktopShell {
public:
    void load() {
        cout << "[DESKTOP] Loaded" << endl;
    }
};

class WindowManager {
public:
    void createWindow(string title) {
        cout << "[WM] Window: " << title << endl;
    }
};

class Compositor {
public:
    void init() {
        cout << "[COMPOSITOR] Started" << endl;
    }
};

class InputManager {
public:
    void listen() {
        cout << "[INPUT] Active" << endl;
    }
};

int main() {

    cout << "====================" << endl;
    cout << " KAIZOR OS START " << endl;
    cout << "====================" << endl;

    systemInit();

    Compositor comp;
    comp.init();

    InputManager input;
    input.listen();

    DesktopShell desktop;
    desktop.load();

    WindowManager wm;
    wm.createWindow("Files");
    wm.createWindow("Browser");
    wm.createWindow("Settings");

    cout << "\n[KAIZOR OS READY]" << endl;

    #include <iostream>
using namespace std;

class DisplayServer {

public:

    void init() {
        cout << "[DISPLAY] Server Started" << endl;
        cout << "[DISPLAY] Screen Buffer Ready" << endl;
    }

    void createSurface(string name) {
        cout << "[DISPLAY] Surface Created: " << name << endl;
    }
};

    return 0;
}
