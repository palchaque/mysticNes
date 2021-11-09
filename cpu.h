#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <iostream>

#include "memory.h"

namespace mysticNes {

    class CPU
    {
    public:
        CPU(Memory &mem): memory(mem)
        {
            pc = 0x8000;
            x = 0;
            y = 0;
            cyclesCounter = 0;
            s = 0xFF; // from top to bottom
            fC = false;
            fZ = false;
            fI = false;
            fB = false;
            fV = false;
            fN = false;
        };
        void step();
    private:
        Memory memory;
        uint16_t cyclesCounter;
        uint16_t pc; //program counter
        uint8_t a; //accumulator register
        uint8_t x; //general purpose registers (x & y)
        uint8_t y;
        uint8_t s; //stack pointer

        //flags register representation
        bool fC; //carry flag
        bool fZ; //zero flag
        bool fD; //decimal flag
        bool fI; //interrupt flag
        bool fB; //break flag
        bool fV; //oveflow flag
        bool fN; //nagative flag

        //CLC - drop C flag
        //CLD - drop D flag
        //CLI - drop I flag
        //CLV - drop V flag
        //
        void setFlag(bool &flag, bool value);

        void loadIntoRegister(uint8_t &reg, uint8_t value);

        //JMP
        void jump(uint16_t address);

        //JSR
        void jumpToSubroutine(uint16_t address);

        //DEX
        //DEY
        void decrementRegisterValue(uint8_t &reg);

        //For ABS addressing mode instructions
        uint16_t getAbsoluteAddress(uint16_t);



    };

}

#endif // CPU_H
