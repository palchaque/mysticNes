#ifndef EMULATOR_H
#define EMULATOR_H

#include <cstdint>
#include <fstream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <memory>

#include <SDL.h>

#include "interrupts.h"
#include "cpu.h"
#include "memory.h"
#include "ppu.h"
#include "input.h"

namespace mysticNes {

    class Emulator
    {
    public:
        Emulator();
        void run();
        void loadROM(const char* romPath);
        void init();
        uint8_t readFromMemory(uint16_t address, uint16_t &cyclesCounter);
        ~Emulator()
        {
            SDL_DestroyTexture(emulatorTexture);
            SDL_DestroyRenderer(emulatorRenderer);
            SDL_DestroyWindow(emulatorWindow);
            SDL_Quit();
        }
    private:
        std::shared_ptr<Memory> emulatorMemory;
        std::unique_ptr<CPU> emulatorCPU;
        std::shared_ptr<Input> emulatorInput;
        std::shared_ptr<Interrupts> emulatorInterrupts;
        std::unique_ptr<PPU> emulatorPPU;
        SDL_Window *emulatorWindow;
        SDL_Renderer *emulatorRenderer;
        SDL_Texture *emulatorTexture;
    };

}

#endif // EMULATOR_H
