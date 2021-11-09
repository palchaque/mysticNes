#ifndef EMULATOR_H
#define EMULATOR_H

#include <cstdint>
#include <fstream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <memory>

#include "cpu.h"
#include "memory.h"

namespace mysticNes {

    class Emulator
    {
    public:
        Emulator();
        void step();
        void loadROM(const char* romPath);
        void init();
        void setMemory(Memory&&);
        uint8_t readFromMemory(uint16_t address, uint16_t &cyclesCounter);
        uint16_t cyclesCounter;
    private:
        std::shared_ptr<CPU> emulatorCPU;
        Memory emulatorMemory;
    };

}

#endif // EMULATOR_H
