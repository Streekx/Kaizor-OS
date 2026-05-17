#include "kaizor_init.h"

int main() {
    KaizorInit init;
    init.boot();

    // In real OS, PID1 never exits.
    while(true) { }

    return 0;
}
