#include <iostream>
#include <memory>
#include "emulator.h"

int main(int argc, char* argv[])
{
    bool running = true;
    mysticNes::Emulator emulator;
    emulator.init();

    emulator.loadROM(argv[1]);

    while(running)
    {
        emulator.step();
    }

    return 0;
}
