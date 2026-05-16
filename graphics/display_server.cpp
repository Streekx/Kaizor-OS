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
