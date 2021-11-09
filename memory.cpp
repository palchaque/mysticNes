#include "memory.h"

using namespace mysticNes;

Memory::Memory()
{
    memoryData = std::make_shared<std::array<uint8_t, 65535>>();
}

uint8_t Memory::readFromMemory(uint16_t address)
{
    return memoryData->at(address);
}

void Memory::storeIntoMemory(uint16_t address, uint8_t value)
{
    memoryData->at(address) = value;
}

void Memory::loadROM(std::vector<unsigned char> romData)
{
    for(int romIndex = 0x8000; romIndex<memoryData->size(); romIndex++)
    {
        memoryData->at(romIndex) = romData.at(romIndex-0x8000);
    }
}

