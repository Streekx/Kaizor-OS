#include <iostream>
using namespace std;

class InputManager {

public:

    void listen() {
        cout << "[INPUT] Listening to mouse & keyboard events..." << endl;
    }

    void click(int x, int y) {
        cout << "[INPUT] Click at (" << x << "," << y << ")" << endl;
    }
};
