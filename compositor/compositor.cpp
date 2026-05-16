#include <iostream>
using namespace std;

class Compositor {

public:

    void init() {
        cout << "[COMPOSITOR] Graphics Layer Initialized" << endl;
    }

    void drawFrame() {
        cout << "[COMPOSITOR] Drawing Frame..." << endl;
    }
};
