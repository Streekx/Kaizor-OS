#include <iostream>
using namespace std;

// ================= CORE =================
void systemInit() {
    cout << "[CORE] Init Done" << endl;
}

// ================= DESKTOP =================
class DesktopShell {
public:
    void load() {
        cout << "[DESKTOP] Loaded" << endl;
        cout << "[DESKTOP] Wallpaper Applied" << endl;
        cout << "[DESKTOP] Dock Ready" << endl;
    }
};

// ================= WINDOW MANAGER =================
class WindowManager {
public:
    void createWindow(string title) {
        cout << "[WM] Window Created: " << title << endl;
    }
};

// ================= COMPOSITOR =================
class Compositor {
public:
    void init() {
        cout << "[COMPOSITOR] Graphics Layer Started" << endl;
    }
};

// ================= INPUT =================
class InputManager {
public:
    void listen() {
        cout << "[INPUT] Mouse + Keyboard Active" << endl;
    }
};

// ================= DISPLAY =================
class DisplayServer {
public:
    void init() {
        cout << "[DISPLAY] Server Started" << endl;
        cout << "[DISPLAY] Framebuffer Ready" << endl;
    }

    void createSurface(string name) {
        cout << "[DISPLAY] Surface: " << name << endl;
    }
};

// ================= MAIN =================
int main() {

    cout << "========================" << endl;
    cout << "     KAIZOR OS BOOT     " << endl;
    cout << "========================" << endl;

    // CORE INIT
    systemInit();

    // GRAPHICS PIPELINE
    Compositor comp;
    comp.init();

    DisplayServer display;
    display.init();
    display.createSurface("Kaizor Desktop Surface");

    // INPUT SYSTEM
    InputManager input;
    input.listen();

    // DESKTOP
    DesktopShell desktop;
    desktop.load();

    // WINDOW SYSTEM
    WindowManager wm;
    wm.createWindow("Files");
    wm.createWindow("Browser");
    wm.createWindow("Settings");

    cout << "\n[KAIZOR OS READY]" << endl;

    return 0;
}
