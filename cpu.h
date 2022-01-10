#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <iostream>
#include <bitset>
#include <memory>

#include "memory.h"
#include "interrupts.h"
#include "input.h"

//http://www.6502.org/tutorials/6502opcodes.html
namespace mysticNes {

    class CPU
    {
    public:
        CPU(Memory *mem, Interrupts *inter):memory(mem), interrupt(inter)
        {
            pc = 0xc000;
            x = 0;
            y = 0;
            cpuCycleCounter = 0;
            s = 0xFF; // from top to bottom
            fC = false;
            fZ = false;
            fI = false;
            fD = false;
            fB = false;
            fU = false;
            fV = false;
            fN = false;
            inInterrupt = false;            
        };
        uint16_t step();
        void reset();
        //void setInterrupts(std::shared_ptr<Interrupts> inter);

    private:       
        Memory *memory;
        Interrupts *interrupt;
        uint64_t cpuCycleCounter; //total cycles counter
        uint16_t cyclesCounter; //current operation cycle counter for synchronization
        uint16_t pc; //program counter
        uint8_t a; //accumulator register
        uint8_t x; //general purpose registers (x & y)
        uint8_t y;
        uint8_t s; //stack pointer                

        //flags register representation
        bool fC; //carry flag
        bool fZ; //zero flag
        bool fI; //interrupt flag
        bool fD; //decimal flag
        bool fB; //break flag
        bool fU; //unused flag
        bool fV; //oveflow flag
        bool fN; //nagative flag

        uint8_t read(u_int16_t address);
        void store(uint16_t address, uint8_t value);

        //CLC - drop C flag
        //CLD - drop D flag
        //CLI - drop I flag
        //CLV - drop V flag
        //
        void setFlag(bool &flag, bool value, bool incrementCounter);

        //returns all flags in register representation
        uint8_t getStatusRegister();

        void restoreStatusRegister(uint8_t value);

        //ADC operations
        void addToRegister(uint8_t &reg, uint8_t value);

        //SBC
        void substractFromRegister(uint8_t &reg, uint8_t value);

        void loadIntoRegister(uint8_t &reg, uint8_t value);

        //For Branch instructions
        void branch(uint16_t address);

        //JMP
        void jump(uint16_t address);

        //JSR
        void jumpToSubroutine(uint16_t address);

        //DEX
        //DEY
        void decrementRegisterValue(uint8_t &reg);

        //INX
        //INY
        void incrementRegisterValue(uint8_t &reg);

        //AND
        void bitwiseAndWithRegister(uint8_t &reg, uint8_t value);

        //ASL
        void arithmeticShiftLeftRegister(uint8_t &reg);
        void arithmeticShiftLeft(uint16_t address);

        //BIT
        void bitwiseAndWithAccumulator(uint8_t value);

        //BRK
        void brkInterrupt();

        //NMI
        void nmiInterrupt();

        //CMP CPX CPY
        void compareWithRegister(uint8_t &reg, uint8_t value);

        //DEC
        void decrementMemory(uint16_t address);

        //EOR
        void bitwiseXorWithAccumulator(uint8_t value);

        //ORA
        void bitwiseOrWithAccumulator(uint8_t value);

        //INC
        void incrementMemory(uint16_t address);

        //LSR
        void logicalShiftRightAccumulator();
        void logicalShiftRight(uint16_t address);

        //ROL
        void rotateLeftAccumulator();
        void rotateLeft(uint16_t address);

        //ROR
        void rotateRightAccumulator();
        void rotateRight(uint16_t address);

        //RTI
        void returnFromInterrupt();

        //RTS
        void returnFromSubroutine();

        //ABS addressing mode instructions
        uint16_t getAbsoluteAddress(uint16_t&);

        //For ABSX/ABSY
        uint16_t getAbsoluteAddressWithRegisterOffset(uint16_t&, uint8_t&reg);
        //uint16_t getIndirectAddressWithRegisterOffset(uint16_t&, uint8_t&reg);

        uint8_t getValueFromZeroPage(uint16_t); //ZP Addressing
        uint8_t getValueFromZeroPageWithRegisterOffset(u_int16_t, uint8_t &reg); //ZPX ZPY Addresing
        uint16_t getAddressInZeroPageWithRegisterOffset(u_int16_t, uint8_t &reg); //ZPX, ZPY for instructions with store in memory

        uint8_t getIndirectValue(uint16_t); //IND Addressing
        uint16_t getIndirectAddress(uint16_t);
        uint16_t getIndirectAddressWithRegisterOffsetIndX(uint16_t, uint8_t &reg);
        uint16_t getIndirectAddressWithRegisterOffsetIndY(uint16_t, uint8_t &reg);
        uint8_t getIndirectValueWithRegisterOffset(uint16_t, uint8_t &reg); //INDX Addressing
        uint8_t getIndirectValueFromAddressWithRegisterOffset(uint16_t, uint8_t &reg); //INDY Addressing

        bool inInterrupt;
    };

}

#endif // CPU_H
