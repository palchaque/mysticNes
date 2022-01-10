#include "ppu.h"

using namespace mysticNes;

void PPU::setMemory(Memory *mem)
{
    memory = mem;
}

void PPU::setInterrupts(Interrupts *inter)
{
    interrupt = inter;
}

void PPU::loadDefaultPallete()
{
    const std::array<uint8_t, 16> defaultBGPallete{0x0f, 0x2C, 0x38, 0x12, 0x0f, 0x27, 0x27, 0x27, 0x0f, 0x30, 0x30, 0x30, 0x0f, 0x00, 0x04, 0x2c};
    const std::array<uint8_t, 16> defaultSpritePallete{0x0f, 0x25, 0x34, 0x03, 0x0f, 0x04, 0x00, 0x14, 0x0f, 0x3a, 0x00, 0x02, 0x0f, 0x20, 0x2c, 0x08};

    for (int i = 0; i < 16; i++)
    {
        vram->at(0x3F00+i) = defaultBGPallete.at(i);
        vram->at(0x3F10+i) = defaultSpritePallete.at(i);
        vram->at(0x3F20+i) = defaultBGPallete.at(i);
        vram->at(0x3F30+i) = defaultSpritePallete.at(i);
        vram->at(0x3F40+i) = defaultBGPallete.at(i);
        vram->at(0x3F50+i) = defaultSpritePallete.at(i);
        vram->at(0x3F60+i) = defaultBGPallete.at(i);
        vram->at(0x3F70+i) = defaultSpritePallete.at(i);
        vram->at(0x3F80+i) = defaultBGPallete.at(i);
        vram->at(0x3F90+i) = defaultSpritePallete.at(i);
        vram->at(0x3FA0+i) = defaultBGPallete.at(i);
        vram->at(0x3FB0+i) = defaultSpritePallete.at(i);
        vram->at(0x3FC0+i) = defaultBGPallete.at(i);
        vram->at(0x3FD0+i) = defaultSpritePallete.at(i);
        vram->at(0x3FE0+i) = defaultBGPallete.at(i);
        vram->at(0x3FF0+i) = defaultSpritePallete.at(i);
    }

}

void PPU::updateRegisters()
{
    //CTRL
    if (memory->lastWrittenAddress == 0x2000)
    {
        ctrl = memory->readFromMemory(0x2000);

        generateNMI = ctrl>>7;
        if(((ctrl >> 4) & 0x1) == 1) bgAddress = 0x1000;
        if(((ctrl >> 3) & 0x1) == 1) spriteAddress = 0x1000;

        if(((ctrl >> 2) & 0x1) == 1)
        {
            vramIncrementValue = 32;
        }
        else
        {
            vramIncrementValue = 1;
        }

        switch(ctrl & 0x3)
        {
        case (0x00):
            ntAddress = 0x2000;
            break;
        case (0x01):
            ntAddress = 0x2400;
            break;
        case (0x10):
            ntAddress = 0x2800;
            break;
        case (0x03):
            ntAddress = 0x2C00;
            break;
        }
        memory->lastWrittenAddress = 0x0000;
        memory->lastWrittenValue = 0x00;

    }

    //MASK
    if(memory->lastWrittenAddress == 0x2001)
    {
        mask = memory->readFromMemory(0x2001);

        drawBackground = (mask >> 3) & 0x1;
        drawSprites = (mask >> 4) & 0x1;
        //memory.storeIntoMemory(statusRegisterAddress, memory.lastWrittenValue);
        memory->lastWrittenAddress = 0x0000;
        memory->lastWrittenValue = 0x00;
    }

    //Sprites, address, value, dma
    if (memory->lastWrittenAddress == 0x2003 && spriteWriteToggle)
    {
        uint16_t tempValue = (uint16_t) memory->lastWrittenValue;
        oamAddress = tempValue;
        spriteWriteToggle = false;
        //memory.storeIntoMemory(statusRegisterAddress, memory.lastWrittenValue);
        memory->lastWrittenAddress = 0x0000;
    }

    if (memory->lastWrittenAddress == 0x2003 && !spriteWriteToggle)
    {
        oamAddress = memory->lastWrittenValue << 8;
        spriteWriteToggle = true;
        //memory.storeIntoMemory(statusRegisterAddress, memory.lastWrittenValue);
        memory->lastWrittenAddress = 0x0000;
        memory->lastWrittenValue = 0x00;
    }

    if(memory->lastWrittenAddress == 0x2004)
    {
        oam->at(oamAddress) = memory->lastWrittenValue;
        oamAddress += 1;
        //memory.storeIntoMemory(statusRegisterAddress, memory.lastWrittenValue);
        memory->lastWrittenAddress = 0x0000;
        memory->lastWrittenValue = 0x00;
    }

    //Scrolling
    if(memory->lastWrittenAddress == 0x2005 && !scrollingWriteToggle)
    {
        scrollingWriteToggle = true;
        yScrolling = memory->lastWrittenValue;
        memory->lastWrittenAddress = 0x0000;
        memory->lastWrittenValue = 0x00;
    }

    if(memory->lastWrittenAddress == 0x2005 && scrollingWriteToggle)
    {
        scrollingWriteToggle = false;
        xScrolling = memory->lastWrittenValue;
        memory->lastWrittenAddress = 0x0000;
        memory->lastWrittenValue = 0x00;
    }

    //PPU memory address, value
    if (memory->lastWrittenAddress == 0x2006 && writeToggle)
    {
        uint16_t tempValue = (uint16_t) (tempAddress) + memory->lastWrittenValue;
        tempAddress = tempValue;
        vramAddress = tempAddress;
        writeToggle = false;
        //memory.storeIntoMemory(statusRegisterAddress, memory.lastWrittenValue);
        memory->lastWrittenAddress = 0x0000;
    }

    if (memory->lastWrittenAddress == 0x2006 && !writeToggle)
    {
        tempAddress = memory->lastWrittenValue << 8;
        writeToggle = true;
        //memory.storeIntoMemory(statusRegisterAddress, memory.lastWrittenValue);
        memory->lastWrittenAddress = 0x0000;
        memory->lastWrittenValue = 0x00;
    }

    if (memory->lastWrittenAddress == 0x2007 && vramAddress!=0x000)
    {
        vram->at(vramAddress) = memory->lastWrittenValue;
        vramAddress += vramIncrementValue;
        memory->storeIntoMemory(statusRegisterAddress, vram->at(vramAddress));
        memory->lastWrittenAddress = 0x0000;
        memory->lastWrittenValue = 0x00;
    }

    if(memory->lastWrittenAddress == 0x4014)
    {
        uint16_t memoryBegin = memory->lastWrittenValue << 8;
        uint16_t memoryEnd = memoryBegin + 0xFF;

        auto memoryRange = memory->getMemoryRange(memoryBegin, memoryEnd);

        for (int i = 0; i < memoryRange.size(); i++)
        {
            oam->at(i) = memoryRange.at(i);
        }
        //memory.storeIntoMemory(statusRegisterAddress, memory.lastWrittenValue);
        memory->lastWrittenAddress = 0x0000;
        memory->lastWrittenValue = 0x00;
    }

}

void PPU::drawBackgroundPicture()
{

    int currentPosition = 0;

    for(int r = 0; r < 30; r++)
    {
        for (int col = 0; col < 32; col++)
        {
            uint16_t tileIndex = vram->at(ntAddress + (r * 32) + (col));
            uint16_t tileAddress = bgAddress + (tileIndex * 0x10);

            for(int tileRow = 0; tileRow < 8; tileRow++)
            {

                uint8_t firstPattern = vram->at(tileAddress + tileRow);
                uint8_t secondPattern = vram->at(tileAddress + tileRow + 8);

                uint16_t attributesAddress = ntAddress + 0x3C0; //Begin of attributes data per nametable
                uint16_t currentAttributeAddress = attributesAddress+(r / 4 *8)+(col / 4); //address per screen section
                uint8_t attributeData = vram->at(currentAttributeAddress);
                uint16_t palleteOffset = bgPalleteAddress+((attributeData & 0x3) * 4);

                currentPosition = (r*256*8)+(col*8)+(tileRow*256);

                for(int bit = 7; bit >= 0; bit--)
                {
                    uint8_t pixel = (firstPattern >> bit & 0x01) + ((secondPattern >> bit & 0x01) * 2);
                    framebuffer->at(currentPosition) = Pallete[vram->at(palleteOffset + pixel)];
                    currentPosition++;
                }

            }

        }
    }

    SDL_UpdateTexture(texture, NULL, framebuffer->data(), textureRowSize);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

}

void PPU::drawSpritesPicture()
{

    for(int i = 0; i<64; i++)
    {
        uint8_t spriteIndex = i * 4;
        uint8_t spriteYPosition = oam->at(spriteIndex);
        uint8_t spriteNum = oam->at(spriteIndex+1);
        uint8_t spriteAttributes = oam->at(spriteIndex+2);
        uint8_t spriteXPosition = oam->at(spriteIndex+3);

        if(spriteYPosition == 0xff || spriteXPosition == 0) continue;

        uint16_t tileAddress = spriteAddress+(spriteNum<<4);

        uint8_t flip = spriteAttributes >> 6 & 0x3;

        uint16_t paletteOffset = 0x3f10 + ((spriteAttributes & 3) * 4);

        switch (flip) {
        case 0x00: //no flip
            for (int tileRow = 0; tileRow < 8; tileRow++) {

                uint8_t firstPattern = vram->at(tileAddress + tileRow);
                uint8_t secondPattern = vram->at(tileAddress + tileRow + 8);
                int currentXPosition = 0;
                int currentYPosition = 0;
                for(int bit = 7; bit >= 0; bit--)
                {
                    uint8_t pixel = (firstPattern >> bit & 0x01) + ((secondPattern >> bit & 0x01) * 2);
                    if(((spriteYPosition + 1 + tileRow) * 256) + ((spriteXPosition + currentXPosition)) > framebuffer->size()) continue;
                    if(pixel != 0) framebuffer->at(((spriteYPosition + 1 + tileRow) * 256) + ((spriteXPosition + currentXPosition))) = Pallete[vram->at(paletteOffset + pixel)];
                    currentXPosition++;
                }
                ++currentYPosition;
            }
            break;
        case 0x01:  //horizontal flip
            for (int tileRow = 0; tileRow < 8; tileRow++) {

                uint8_t firstPattern = vram->at(tileAddress + tileRow);
                uint8_t secondPattern = vram->at(tileAddress + tileRow + 8);
                int currentXPosition = 0;
                int currentYPosition = 0;
                for(int bit = 0; bit < 8; bit++)
                {
                    uint8_t pixel = (firstPattern >> bit & 0x01) + ((secondPattern >> bit & 0x01) * 2);
                    if(((spriteYPosition + 1 + tileRow) * 256) + ((spriteXPosition + currentXPosition)) > framebuffer->size()) continue;
                    if(pixel != 0) framebuffer->at(((spriteYPosition + 1 + tileRow) * 256) + ((spriteXPosition + currentXPosition))) = Pallete[vram->at(paletteOffset + pixel)];
                    currentXPosition++;
                }
                ++currentYPosition;
            }
            break;
        case 0x10:  //vertical flip
            break;
        case 0x11:  //horizontal & vertical flip
            break;
        }

    }

    SDL_UpdateTexture(texture, NULL, framebuffer->data(), textureRowSize);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

}

void PPU::loadVRAM()
{
    *vram = memory->getVRAM();
    loadDefaultPallete();
    ppuCycles += 27;
    ++frameCounter;
}

uint8_t PPU::getStatusRegister()
{
    uint8_t status = 0;
    status += 0 << 0;	// Unused
    status += 0 << 1;	// Unused
    status += 0 << 2;	// Unused
    status += 0 << 3;	// Unused
    status += 0 << 4;	// Unused
    status += spriteOverflow << 5; //Overflow
    status += sprite0Hit << 6;	// 0 hit
    status += vBlankStarted << 7;	// vBlank started

    return status;
}

void PPU::setWindow(SDL_Window *emulatorWindow)
{
    window = emulatorWindow;
}

void PPU::setRenderer(SDL_Renderer *emulatorRenderer)
{
    renderer = emulatorRenderer;
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 256, 240);

}

void PPU::step(uint16_t cpuCycles)
{
    updateRegisters();

    uint16_t localPPUCycle = cpuCycles*3;

    if(localPPUCycle == 0 ) return;

    for (int localCycle = 0; localCycle < localPPUCycle; localCycle++)
    {
        ppuCycles++;

        //end vBlank
        if(scanlineCounter == 261 && vBlankStarted && ppuCycles == 341)
        {
            vBlankStarted = false;
            memory->storeIntoMemory(0x2002, getStatusRegister());
            interrupt->setNmiOccurred(false);
            scanlineCounter = 0;
            ++frameCounter;
        }

        //begin vBlank
        if(scanlineCounter == 241 && ppuCycles == 1 && !vBlankStarted)
        {
            vBlankStarted = true;

            if(generateNMI) interrupt->setNmiOccurred(true);

            memory->storeIntoMemory(0x2002, getStatusRegister());

            if(drawBackground) drawBackgroundPicture();
            if(drawSprites) drawSpritesPicture();
        }

        if(ppuCycles >= 341)
        {
            ++scanlineCounter;
            ppuCycles = ppuCycles - 341;
        }

    }

}
