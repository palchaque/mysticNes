#ifndef MEMORY_H
#define MEMORY_H

#include <memory>
#include <vector>
namespace mysticNes {

    class Memory
    {
    public:
        Memory();
        uint8_t readFromMemory(uint16_t address);
        void storeIntoMemory(uint16_t address, uint8_t value);
        void loadROM(std::vector<unsigned char>);
    private:
        std::shared_ptr<std::array<uint8_t, 65535>> memoryData;
    };

}

#endif // MEMORY_H
