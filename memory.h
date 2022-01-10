#ifndef MEMORY_H
#define MEMORY_H

#include <memory>
#include <vector>

#include "input.h"

namespace mysticNes {

    class Memory
    {
    public:
        Memory()
        {
            memoryData = std::make_shared<std::array<uint8_t, 65536>>();
            vramData = std::make_shared<std::array<uint8_t, 16385>>();
        };

        uint8_t readFromMemory(uint16_t address);
        void storeIntoMemory(uint16_t address, uint8_t value);
        void loadROM(std::vector<unsigned char>);
        std::vector<uint8_t> getMemoryRange(uint16_t startAddress, uint16_t endAddress);
        std::array<uint8_t, 16385> getVRAM();
        uint16_t lastWrittenAddress;
        uint8_t lastWrittenValue;
        void setInput(std::shared_ptr<Input>);
        //std::shared_ptr<std::array<uint8_t, 65535>> romData;
        std::shared_ptr<Input> emulatorInput;

    private:
        std::shared_ptr<std::array<uint8_t, 65536>> memoryData;
        std::shared_ptr<std::array<uint8_t, 16385>> vramData;
    };

}

#endif // MEMORY_H
