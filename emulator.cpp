#include "emulator.h"

using namespace mysticNes;

Emulator::Emulator()
{

    emulatorCPU = std::make_shared<CPU>(emulatorMemory);
}

void Emulator::init()
{

}

void Emulator::step()
{
    emulatorCPU->step();
}

void Emulator::loadROM(const char *romPath)
{
     std::ifstream input(romPath, std::ios::binary);
     input.seekg(16); //skip the header
     input.unsetf(std::ios::skipws);
     std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
     emulatorMemory.loadROM(std::move(buffer));
}

uint8_t Emulator::readFromMemory(uint16_t address, uint16_t &cyclesCounter)
{
    ++cyclesCounter;
    return emulatorMemory.readFromMemory(address);
}
