#include "emulator.h"

using namespace mysticNes;

Emulator::Emulator()
{

}

void Emulator::init()
{
    emulatorInterrupts = std::make_shared<Interrupts>();
    emulatorMemory = std::make_shared<Memory>();
    emulatorCPU = std::make_unique<CPU>(emulatorMemory.get(), emulatorInterrupts.get());
    emulatorPPU = std::make_unique<PPU>();

    emulatorInput = std::make_shared<Input>();
    emulatorMemory->setInput(emulatorInput);

    emulatorPPU->setInterrupts(emulatorInterrupts.get());
    emulatorPPU->setMemory(emulatorMemory.get());

    SDL_Init(SDL_INIT_VIDEO);
    emulatorWindow = SDL_CreateWindow("MysticNES", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 512, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS);

    emulatorRenderer = SDL_CreateRenderer(emulatorWindow, -1, SDL_RENDERER_ACCELERATED);
    emulatorPPU->setWindow(emulatorWindow);
    emulatorPPU->setRenderer(emulatorRenderer);

}

void Emulator::run()
{
    bool running = true;

    while(running)
    {
        unsigned int cyclesCounter = emulatorCPU->step();
        emulatorPPU->step(cyclesCounter);
    }

}

void Emulator::loadROM(const char *romPath)
{
     std::ifstream input(romPath, std::ios::binary);
     input.seekg(16); //skip the header
     input.unsetf(std::ios::skipws);
     std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
     emulatorMemory->loadROM(std::move(buffer));

     emulatorPPU->loadVRAM();

     emulatorCPU->reset();

}

uint8_t Emulator::readFromMemory(uint16_t address, uint16_t &cyclesCounter)
{
    ++cyclesCounter;
    return emulatorMemory->readFromMemory(address);
}
