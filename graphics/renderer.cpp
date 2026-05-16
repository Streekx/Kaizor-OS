#include <iostream>
using namespace std;

class Renderer {

public:

    void clearScreen() {
        cout << "[RENDER] Screen Cleared" << endl;
    }

    void drawWindow(string title, int x, int y) {
        cout << "[RENDER] Window: " << title
             << " at (" << x << "," << y << ")" << endl;
    }
};
