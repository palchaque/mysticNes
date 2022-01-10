#ifndef PPU_H
#define PPU_H

#include <cstdint>
#include <memory>
#include <SDL.h>
#include <iostream>
#include <bitset>

#include "memory.h"
#include "interrupts.h"

//PPU - Picture Processing Unit
//OAM - Object Attribute Memory
namespace mysticNes {

    class PPU
    {
    public:
        PPU()
        {
            ctrl = 0;
            mask = 0;
            status = 0;
            oamAddress = 0;
            oamData = 0;
            scroll = 0;
            address = 0;
            data = 0;
            oamDma = 0;
            vram = std::make_shared<std::array<uint8_t, 16385>>();
            oam = std::make_shared<std::array<uint8_t, 256>>();
            framebuffer = std::make_unique<std::array<uint32_t, 256*240>>();
            ntAddress = 0x2000;
            vramAddress = 0x0000;
            tempAddress = 0x0000;
            bgAddress = 0x0000;
            spriteAddress = 0x0000;
            writeToggle = false;
            scrollingWriteToggle = false;
            xScrolling = 0;
            yScrolling = 0;
            spriteWriteToggle = false;
            drawBackground = false;
            drawSprites = false;
            ppuCycles = 0;
            scanlineCounter = 0;
            frameCounter = 0;
            vBlankStarted = false;
            generateNMI = false;
            vramIncrementValue = 1;
        };
        void drawScreen();
        void step(uint16_t);
        void loadVRAM();
        void setMemory(Memory *mem);
        void setInterrupts(Interrupts *inter);
        void setWindow(SDL_Window *);
        void setRenderer(SDL_Renderer *);
    private:
        const uint16_t statusRegisterAddress = 0x2002;
        void loadDefaultPallete();
        void updateRegisters();
        void setStatusRegister();

        void drawBackgroundPicture();
        void drawSpritesPicture();

        uint8_t getStatusRegister();

        uint8_t ctrl; //PPUCTRL register
        uint8_t mask; //PPUMASK register
        uint8_t status; //PPUSTATUS register
        uint8_t oamAddress; //OAMADDR register
        uint8_t oamData; //OAMDATA register
        uint8_t scroll; //PPUSCROLL register
        uint8_t address; //PPUADDR register
        uint8_t data; //PPUDATA register
        uint8_t oamDma; //OAMDMA register

        bool generateNMI; //bit 7 in ctrl

        bool drawBackground;
        bool drawSprites;
        bool writeToggle;

        bool scrollingWriteToggle;
        uint8_t xScrolling;
        uint8_t yScrolling;

        bool spriteWriteToggle;

        bool vBlankStarted;
        bool sprite0Hit;
        bool spriteOverflow;

        uint16_t vramAddress;
        uint16_t ntAddress;
        uint16_t bgAddress;
        uint16_t spriteAddress;
        uint16_t tempAddress;

        uint8_t vramIncrementValue; //bit 2 in 0x2000

        Interrupts *interrupt;
        Memory *memory;
        uint16_t ppuCycles;
        unsigned short int scanlineCounter;
        unsigned int frameCounter;
        std::shared_ptr<std::array<uint8_t, 16385>> vram;
        std::shared_ptr<std::array<uint8_t, 256>> oam;
        SDL_Window *window;
        SDL_Renderer *renderer;
        SDL_Texture *texture;
        const int textureRowSize = 256*sizeof(uint32_t);

        const uint16_t bgPalleteAddress = 0x3F00;
        const uint16_t spritePalleteAddress = 0x3F10;

        std::unique_ptr<std::array<uint32_t, 256*240>> framebuffer;  //current frame - rgb per pixel
        const uint32_t Pallete[64] = {0x757575, 0x0000FC, 0x0000BC, 0x9F0047,
                                      0x940084, 0xA80020, 0xA81000, 0x000B7F,
                                      0x503000, 0xBCBCBC, 0x006800, 0x005800,
                                      0x4240FF, 0x000000, 0x000000, 0x000000,
                                      0xBCBCBC, 0x0078F8, 0x4240FF, 0x6844FC,
                                      0xD800CC, 0xE40058, 0xF83800, 0xE45C10,
                                      0xAC7C00, 0x00B800, 0x00A800, 0xEA9E22,
                                      0x008888, 0xFFFEFF, 0x000000, 0x000000,
                                      0xF8F8F8, 0x3CBCFC, 0x6888FC, 0x9878F8,
                                      0xF878F8, 0xF85898, 0xF87858, 0xFCA044,
                                      0xF8B800, 0xB8F818, 0x58D854, 0x58F898,
                                      0x48CDDE, 0x787878, 0x000000, 0x000000,
                                      0xFCFCFC, 0xA4E4FC, 0xB8B8F8, 0xD8B8F8,
                                      0xF8B8F8, 0xF8A4C0, 0xF0D0B0, 0xFCE0A8,
                                      0xF8D878, 0xD8F878, 0xB8F8B8, 0xB8F8D8,
                                      0x00FCFC, 0xF8D8F8, 0x000000, 0x000000};

    };


}

#endif // PPU_H
