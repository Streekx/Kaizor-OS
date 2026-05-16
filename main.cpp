#include <iostream>
#include <vector>

using namespace std;

// ================= WINDOW STRUCT =================
struct Window {
    int id;
    string title;
    int x, y;
    int width, height;
    bool focused;
};

// ================= CORE =================
void systemInit() {
    cout << "[CORE] Init Done" << endl;
}

// ================= DISPLAY =================
class DisplayServer {
public:
    void init() {
        cout << "[DISPLAY] Server Started" << endl;
        cout << "[DISPLAY] Framebuffer Ready" << endl;
    }

    void refreshScreen() {
        cout << "[DISPLAY] Screen Refreshed" << endl;
    }
};

// ================= COMPOSITOR =================
class Compositor {
public:
    void init() {
        cout << "[COMPOSITOR] Graphics Layer Started" << endl;
    }

    void drawWindow(Window &w) {

        cout << "[DRAW] "
             << w.title
             << " Pos(" << w.x << "," << w.y << ") "
             << "Size(" << w.width << "x" << w.height << ")";

        if (w.focused)
            cout << " [FOCUSED]";

        cout << endl;
    }
};

// ================= INPUT =================
class InputManager {
public:
    void listen() {
        cout << "[INPUT] Mouse + Keyboard Active" << endl;
    }
};

// ================= DESKTOP =================
class DesktopShell {
public:
    void load() {
        cout << "[DESKTOP] Wallpaper Loaded" << endl;
        cout << "[DESKTOP] Dock Started" << endl;
    }
};

// ================= WINDOW MANAGER =================
class WindowManager {

private:
    vector<Window> windows;
    int nextId = 1;

public:

    void createWindow(string title, int x, int y) {

        Window w;

        w.id = nextId++;
        w.title = title;

        w.x = x;
        w.y = y;

        w.width = 400;
        w.height = 300;

        w.focused = false;

        windows.push_back(w);

        cout << "[WM] Window Created: " << title << endl;
    }

    void focusWindow(int id) {

        for (auto &w : windows) {
            w.focused = (w.id == id);
        }

        cout << "[WM] Focus Changed -> " << id << endl;
    }

    void moveWindow(int id, int newX, int newY) {

        for (auto &w : windows) {

            if (w.id == id) {

                w.x = newX;
                w.y = newY;

                cout << "[WM] Window Moved: "
                     << w.title
                     << " -> (" << newX << "," << newY << ")"
                     << endl;
            }
        }
    }

    void resizeWindow(int id, int width, int height) {

        for (auto &w : windows) {

            if (w.id == id) {

                w.width = width;
                w.height = height;

                cout << "[WM] Window Resized: "
                     << w.title
                     << " -> "
                     << width << "x" << height
                     << endl;
            }
        }
    }

    void render(Compositor &comp) {

        cout << "\n========== WINDOWS ==========" << endl;

        for (auto &w : windows) {
            comp.drawWindow(w);
        }

        cout << "=============================\n" << endl;
    }
};

// ================= MAIN =================
int main() {

    cout << "========================" << endl;
    cout << "      KAIZOR OS         " << endl;
    cout << "========================\n" << endl;

    // CORE
    systemInit();

    // DISPLAY
    DisplayServer display;
    display.init();

    // GRAPHICS
    Compositor compositor;
    compositor.init();

    // INPUT
    InputManager input;
    input.listen();

    // DESKTOP
    DesktopShell desktop;
    desktop.load();

    // WINDOW SYSTEM
    WindowManager wm;

    wm.createWindow("Files", 100, 100);
    wm.createWindow("Browser", 300, 150);
    wm.createWindow("Settings", 500, 200);

    wm.focusWindow(2);

    wm.moveWindow(1, 150, 180);

    wm.resizeWindow(3, 600, 500);

    wm.render(compositor);

    display.refreshScreen();

    cout << "[KAIZOR OS READY]" << endl;

    return 0;
}
