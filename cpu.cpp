#include "cpu.h"

using namespace mysticNes;

void CPU::setFlag(bool &flag, bool value)
{
    flag = value;
}

void CPU::loadIntoRegister(uint8_t &reg, uint8_t value)
{
    reg = value;
    setFlag(fN, value & 1);
    setFlag(fZ, value==0);
}

void CPU::decrementRegisterValue(uint8_t &reg)
{
    reg--;
    setFlag(fN, reg & 1);
    setFlag(fZ, reg==0);
}

void CPU::jump(uint16_t address)
{
    pc = address;
}

void CPU::jumpToSubroutine(uint16_t address)
{
    memory.storeIntoMemory(0x100+s, pc);
    s--;
    pc = address;
}

uint16_t CPU::getAbsoluteAddress(uint16_t pc)
{
    uint8_t lb = memory.readFromMemory(pc);
    ++pc;
    uint8_t hb = memory.readFromMemory(pc);
    return (uint16_t) (hb << 8) | lb;
}

uint8_t CPU::getValueFromZeroPage(uint16_t address)
{
    uint8_t zeroPageAddress = memory.readFromMemory(address);
    return memory.readFromMemory((uint16_t) zeroPageAddress);
}

void CPU::step()
{
    uint8_t opcode = memory.readFromMemory(pc);
    pc++;

    switch(opcode)
    {        
        case 0x78:      //SEI
            setFlag(fI, true);
            break;
        case 0xD8:      //CLD
            setFlag(fD, false);
            break;
        case 0xA2:      //LDX
            loadIntoRegister(x, memory.readFromMemory(pc));
            ++pc;
            break;
        case 0x9A:      //TXS
            loadIntoRegister(x, s);
            s--;
            break;
        case 0xA9:      //LDA IMM
            loadIntoRegister(a, memory.readFromMemory(pc));
            ++pc;
            break;
        case 0xAD:      //LDA ABS
            loadIntoRegister(a, getAbsoluteAddress(pc));
            break;
        case 0xA5:      //LDA ZP
            loadIntoRegister(a, getValueFromZeroPage(pc));
            break;
        case 0xA0:      //LDY IMM
            loadIntoRegister(y, getAbsoluteAddress(pc));
            break;
        case 0x85:      //STA ZP
            memory.storeIntoMemory(memory.readFromMemory(pc), a);
            ++pc;
            break;
        case 0x8D:      //STA ABS
            memory.storeIntoMemory(getAbsoluteAddress(pc), a);
            break;

        case 0x30:      //BMI
            if(fN)
            {
                pc = memory.readFromMemory(pc);
                jump(pc);
            }
            break;
        case 0x10:      //BPL
            if(!fN)
            {
                pc = memory.readFromMemory(pc);
                jump(pc);
            }
            break;


        case 0xD0:      //BNE
            if(!fZ)
            {
                pc = memory.readFromMemory(pc);
                jump(pc);
            }
            break;
        case 0xCA:      //DEX
            decrementRegisterValue(x);
            break;
        case 0x88:      //DEY
            decrementRegisterValue(y);
            break;

    }
}
