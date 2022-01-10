#include "memory.h"

using namespace mysticNes;

void Memory::setInput(std::shared_ptr<Input> in)
{
    emulatorInput = in;
}

uint8_t Memory::readFromMemory(uint16_t address)
{
    if(address == 0x2002)
    {
        uint8_t value = memoryData->at(address);
        storeIntoMemory(address, value & 0x7F);
        return value;
    }

    if(address == emulatorInput->controller1Address)
    {
        memoryData->at(address) = emulatorInput->getCurrentController1State();
    }

    return memoryData->at(address);
}

std::vector<uint8_t> Memory::getMemoryRange(uint16_t startAddress, uint16_t endAddress)
{
    std::vector<uint8_t> memoryRange;
    memoryRange.reserve(endAddress - startAddress);

    for (uint16_t index = startAddress; index <= endAddress; index++)
    {
        memoryRange.emplace_back(memoryData->at(index));
    }

    return memoryRange;
}

std::array<uint8_t, 16385> Memory::getVRAM()
{
    return *vramData;
}

void Memory::storeIntoMemory(uint16_t address, uint8_t value)
{
    memoryData->at(address) = value;
    lastWrittenAddress = address;
    lastWrittenValue = value;
    if(address == emulatorInput->controller1Address)
    {
        emulatorInput->enableDisableControllers(value);
    }

}

void Memory::loadROM(std::vector<unsigned char> romData)
{
    for (int i = 0; i < 0x4000; i++) {
        memoryData->at(0xC000 + i) = romData.at(i);
    }

    if(romData.size() >= 0x8000) //NROM with prg-rom 2
    {
        for (int i = 0; i < 0x8000; i++) {
            memoryData->at(0x8000 + i) = romData.at(i);
        }
    }

    int ntIndex = 0x4000;
    for(int vramIndex = 0; vramIndex<0x2000; vramIndex++)
    {
        vramData->at(vramIndex) = romData.at(vramIndex+ntIndex);
    }

}

