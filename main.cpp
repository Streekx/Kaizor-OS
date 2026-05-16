
#include <iostream>
#include <vector>

using namespace std;

// =====================================================
// WINDOW STRUCT
// =====================================================
struct Window {

    int id;

    string title;

    int x;
    int y;

    int width;
    int height;

    bool focused;
    bool minimized;
};

// =====================================================
// DESKTOP ICON STRUCT
// =====================================================
struct DesktopIcon {

    string name;

    int x;
    int y;
};

// =====================================================
// CORE SYSTEM
// =====================================================
void systemInit() {

    cout << "[CORE] Kaizor Core Started" << endl;
    cout << "[CORE] Memory OK" << endl;
    cout << "[CORE] CPU OK" << endl;
}

// =====================================================
// DISPLAY SERVER
// =====================================================
class DisplayServer {

public:

    void init() {

        cout << "[DISPLAY] Display Server Started" << endl;
        cout << "[DISPLAY] Framebuffer Ready" << endl;
    }

    void refresh() {

        cout << "[DISPLAY] Screen Refreshed" << endl;
    }
};

// =====================================================
// INPUT SYSTEM
// =====================================================
class InputManager {

public:

    void init() {

        cout << "[INPUT] Mouse Active" << endl;
        cout << "[INPUT] Keyboard Active" << endl;
    }

    void click(int x, int y) {

        cout << "[INPUT] Click -> ("
             << x << "," << y << ")"
             << endl;
    }

    void drag(int startX, int startY,
              int endX, int endY) {

        cout << "[INPUT] Drag -> ("
             << startX << "," << startY
             << ") to ("
             << endX << "," << endY
             << ")" << endl;
    }
};

// =====================================================
// COMPOSITOR
// =====================================================
class Compositor {

public:

    void init() {

        cout << "[COMPOSITOR] Graphics Engine Started" << endl;
    }

    void drawWindow(Window &w) {

        cout << "[DRAW WINDOW] "
             << w.title
             << " Pos(" << w.x << "," << w.y << ") "
             << "Size(" << w.width << "x" << w.height << ")";

        if (w.focused)
            cout << " [FOCUSED]";

        if (w.minimized)
            cout << " [MINIMIZED]";

        cout << endl;
    }

    void drawTaskbar() {

        cout << "[DRAW] Taskbar Rendered" << endl;
    }

    void drawDock() {

        cout << "[DRAW] Dock Rendered" << endl;
    }

    void drawWallpaper() {

        cout << "[DRAW] Wallpaper Rendered" << endl;
    }

    void drawDesktopIcon(DesktopIcon &icon) {

        cout << "[DRAW ICON] "
             << icon.name
             << " Pos("
             << icon.x << ","
             << icon.y << ")"
             << endl;
    }
};

// =====================================================
// DESKTOP SHELL
// =====================================================
class DesktopShell {

private:

    vector<DesktopIcon> icons;

public:

    void load() {

        cout << "[DESKTOP] Kaizor Desktop Loaded" << endl;
        cout << "[DESKTOP] Nature Wallpaper Applied" << endl;
        cout << "[DESKTOP] Dock Started" << endl;
        cout << "[DESKTOP] Taskbar Started" << endl;
    }

    void createIcon(string name, int x, int y) {

        DesktopIcon icon;

        icon.name = name;
        icon.x = x;
        icon.y = y;

        icons.push_back(icon);

        cout << "[DESKTOP] Icon Added: "
             << name << endl;
    }

    void renderIcons(Compositor &comp) {

        for (auto &icon : icons) {

            comp.drawDesktopIcon(icon);
        }
    }
};

// =====================================================
// WINDOW MANAGER
// =====================================================
class WindowManager {

private:

    vector<Window> windows;

    int nextId = 1;

public:

    void createWindow(string title,
                      int x,
                      int y,
                      int width,
                      int height) {

        Window w;

        w.id = nextId++;

        w.title = title;

        w.x = x;
        w.y = y;

        w.width = width;
        w.height = height;

        w.focused = false;
        w.minimized = false;

        windows.push_back(w);

        cout << "[WM] Window Created: "
             << title << endl;
    }

    void focusWindow(int id) {

        for (auto &w : windows) {

            w.focused = (w.id == id);
        }

        cout << "[WM] Focus Changed -> "
             << id << endl;
    }

    void moveWindow(int id,
                    int x,
                    int y) {

        for (auto &w : windows) {

            if (w.id == id) {

                w.x = x;
                w.y = y;

                cout << "[WM] Window Moved: "
                     << w.title
                     << endl;
            }
        }
    }

    void resizeWindow(int id,
                      int width,
                      int height) {

        for (auto &w : windows) {

            if (w.id == id) {

                w.width = width;
                w.height = height;

                cout << "[WM] Window Resized: "
                     << w.title
                     << endl;
            }
        }
    }

    void minimizeWindow(int id) {

        for (auto &w : windows) {

            if (w.id == id) {

                w.minimized = true;

                cout << "[WM] Window Minimized: "
                     << w.title
                     << endl;
            }
        }
    }

    void restoreWindow(int id) {

        for (auto &w : windows) {

            if (w.id == id) {

                w.minimized = false;

                cout << "[WM] Window Restored: "
                     << w.title
                     << endl;
            }
        }
    }

    void closeWindow(int id) {

        for (int i = 0; i < windows.size(); i++) {

            if (windows[i].id == id) {

                cout << "[WM] Window Closed: "
                     << windows[i].title
                     << endl;

                windows.erase(windows.begin() + i);

                break;
            }
        }
    }

    void render(Compositor &comp) {

        cout << endl;
        cout << "=========== WINDOWS ===========" << endl;

        for (auto &w : windows) {

            comp.drawWindow(w);
        }

        cout << "================================" << endl;
        cout << endl;
    }
};

// =====================================================
// TASKBAR
// =====================================================
class Taskbar {

public:

    void render() {

        cout << "[TASKBAR] Files | Browser | Settings"
             << endl;
    }
};

// =====================================================
// APP LAUNCHER
// =====================================================
class AppLauncher {

public:

    void openMenu() {

        cout << "[LAUNCHER] Start Menu Opened"
             << endl;

        cout << "[LAUNCHER] Apps:" << endl;

        cout << " - Files" << endl;
        cout << " - Browser" << endl;
        cout << " - Settings" << endl;
    }
};

// =====================================================
// MAIN
// =====================================================
int main() {

    cout << "===================================" << endl;
    cout << "          KAIZOR OS                " << endl;
    cout << "===================================" << endl;
    cout << endl;

    // CORE
    systemInit();

    cout << endl;

    // DISPLAY
    DisplayServer display;
    display.init();

    cout << endl;

    // INPUT
    InputManager input;
    input.init();

    cout << endl;

    // COMPOSITOR
    Compositor compositor;
    compositor.init();

    cout << endl;

    // DESKTOP
    DesktopShell desktop;
    desktop.load();

    desktop.createIcon("Files", 50, 100);
    desktop.createIcon("Browser", 50, 200);
    desktop.createIcon("Settings", 50, 300);

    cout << endl;

    // WINDOW MANAGER
    WindowManager wm;

    wm.createWindow("Files",
                    100, 100,
                    500, 400);

    wm.createWindow("Browser",
                    250, 150,
                    700, 500);

    wm.createWindow("Settings",
                    400, 200,
                    450, 350);

    cout << endl;

    // WINDOW ACTIONS
    wm.focusWindow(2);

    wm.moveWindow(1, 150, 180);

    wm.resizeWindow(3, 600, 500);

    wm.minimizeWindow(1);

    wm.restoreWindow(1);

    cout << endl;

    // INPUT ACTIONS
    input.click(300, 200);

    input.drag(100, 100,
               500, 400);

    cout << endl;

    // RENDERING
    compositor.drawWallpaper();

    compositor.drawDock();

    compositor.drawTaskbar();

    desktop.renderIcons(compositor);

    wm.render(compositor);

    cout << endl;

    // TASKBAR
    Taskbar taskbar;
    taskbar.render();

    cout << endl;

    // APP LAUNCHER
    AppLauncher launcher;
    launcher.openMenu();

    cout << endl;

    // DISPLAY REFRESH
    display.refresh();

    cout << endl;

    cout << "[KAIZOR OS READY]" << endl;

    return 0;
}
