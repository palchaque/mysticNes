#include <iostream>
#include <memory>
#include "emulator.h"

int main(int argc, char* argv[])
{
    mysticNes::Emulator emulator;
    emulator.init();

    emulator.loadROM(argv[1]);

    emulator.run();


    return 0;
}
