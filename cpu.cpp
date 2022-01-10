#include "cpu.h"

using namespace mysticNes;

void CPU::reset()
{
    uint8_t lb = memory->readFromMemory(0xFFFC);
    uint8_t hb = memory->readFromMemory(0xFFFD);
    pc = (uint16_t) (hb << 8) | lb;
    cpuCycleCounter += 8;
}

uint8_t CPU::read(u_int16_t address)
{
    ++cyclesCounter;
    ++pc;
    return memory->readFromMemory(address);
}

void CPU::store(uint16_t address, uint8_t value)
{
    cyclesCounter += 1;
    memory->storeIntoMemory(address, value);
}

void CPU::addToRegister(uint8_t &reg, uint8_t value)
{
    uint16_t sum = (uint16_t)reg + (uint16_t)value + (uint16_t)fC;
    setFlag(fN, sum & 0x80, false);
    setFlag(fZ, (sum & 0x00FF) == 0, false);
    setFlag(fC, sum > 0xFF, false);
    setFlag(fV, (~((uint16_t)reg ^ (uint16_t)value) & ((uint16_t)reg ^ (uint16_t)sum)) & 0x0080, false);

    reg = sum & 0x00FF;
    ++cyclesCounter;

}

void CPU::substractFromRegister(uint8_t &reg, uint8_t value)
{
    uint16_t tempValue = ((uint16_t)value) ^ 0x00FF;

    uint16_t temp = (uint16_t)reg + tempValue + (uint16_t)fC;

    setFlag(fC, temp & 0xFF00, false);
    setFlag(fZ, ((temp & 0x00FF) == 0), false);
    setFlag(fV, (temp ^ (uint16_t)reg) & (temp ^ tempValue) & 0x0080, false);
    setFlag(fN, temp & 0x0080, false);

    reg = temp & 0x00FF;
    ++cyclesCounter;

}

void CPU::setFlag(bool &flag, bool value, bool incrementCounter = true)
{
    flag = value;
    if (incrementCounter) cyclesCounter+=2;
}

uint8_t CPU::getStatusRegister()
{
    uint8_t status = 0;
    status += fC << 0;	// Carry Bit
    status += fZ << 1;	// Zero
    status += fI << 2;	// Disable Interrupts
    status += fD << 3;	// Decimal Mode (unused in this implementation)
    status += fB << 4;	// Break
    status += 1  << 5;  // Unused
    status += fV << 6;	// Overflow
    status += fN << 7;	// Negative
    return status;

}

void CPU::restoreStatusRegister(uint8_t value)
{
    std::bitset<8> statusBitset = (std::bitset<8>) value;

    fC = (bool) statusBitset[0];
    fZ = (bool) statusBitset[1];
    fI = (bool) statusBitset[2];
    fD = (bool) statusBitset[3];
    fB = (bool) statusBitset[4];
    fU = (bool) statusBitset[5];
    fV = (bool) statusBitset[6];
    fN = (bool) statusBitset[7];
}

void CPU::loadIntoRegister(uint8_t &reg, uint8_t value)
{
    reg = value;
    setFlag(fN, value & 0x80, false);
    setFlag(fZ, value==0, false);
    ++cyclesCounter;
}

void CPU::decrementRegisterValue(uint8_t &reg)
{
    reg--;
    setFlag(fN, reg & 0x80, false);
    setFlag(fZ, reg==0, false);
    cyclesCounter += 2;
}

void CPU::incrementRegisterValue(uint8_t &reg)
{
    reg++;
    setFlag(fN, reg & 0x80, false);
    setFlag(fZ, reg==0, false);
    cyclesCounter += 2;
}

void CPU::branch(uint16_t address)
{
    int8_t addresRelative = address; //can be <0
    uint16_t addr_abs = pc + addresRelative;

    if((addr_abs & 0xFF00) != (pc & 0xFF00))
                cyclesCounter++;

    pc = addr_abs;
    pc++;
    ++cyclesCounter;

}

void CPU::jump(uint16_t address)
{
    pc = address;
    cyclesCounter += 2;
}

void CPU::jumpToSubroutine(uint16_t address)
{
    address++;
    store((uint16_t)256+s, (address >> 8) & 0x00FF);
    s--;
    store((uint16_t)256+s, address & 0x00FF);
    s--;

    uint8_t lb = memory->readFromMemory(pc);
    ++pc;
    uint8_t hb = memory->readFromMemory(pc);
    ++pc;

    pc = (uint16_t) (hb << 8) | lb;
    cyclesCounter += 4;
}

uint16_t CPU::getAbsoluteAddress(uint16_t &pc)
{
    uint8_t lb = memory->readFromMemory(pc);
    pc++;
    uint8_t hb = memory->readFromMemory(pc);
    pc++;
    cyclesCounter += 3;
    return (uint16_t) ((hb << 8) | lb);
}

uint16_t CPU::getAbsoluteAddressWithRegisterOffset(uint16_t &address, uint8_t &reg)
{
    uint8_t lb = memory->readFromMemory(pc);
    ++pc;
    uint8_t hb = memory->readFromMemory(pc);
    ++pc;
    cyclesCounter += 3;

    if((address & 0xFF00) != (pc & 0xFF00))
                cyclesCounter++;

    return (uint16_t) ((hb << 8) | lb) + reg;;
}


uint8_t CPU::getValueFromZeroPage(uint16_t address)
{
    uint8_t zeroPageAddress = read(address);
    ++cyclesCounter;
    return memory->readFromMemory((uint16_t) zeroPageAddress);
}

uint8_t CPU::getValueFromZeroPageWithRegisterOffset(u_int16_t address, uint8_t &reg)
{
    uint8_t zeroPageAddress = read(address) + reg;
    cyclesCounter+=2;
    return memory->readFromMemory((uint16_t) zeroPageAddress);
}

uint16_t CPU::getAddressInZeroPageWithRegisterOffset(u_int16_t address, uint8_t &reg)
{
    uint8_t zeroPageAddress = read(address);
    //cyclesCounter+=2;
    return (uint16_t) zeroPageAddress+reg;
}

uint8_t CPU::getIndirectValue(u_int16_t address)
{
    uint8_t lb = read(address);
    uint8_t hb = read(++address);
    ++cyclesCounter;
    return memory->readFromMemory((uint16_t) ((hb << 8) | lb));
}

uint16_t CPU::getIndirectAddress(uint16_t address)
{
    uint8_t lb = read(address);
    uint8_t hb = read(++address);
    ++cyclesCounter;
    uint16_t tempAddress = (uint16_t) ((hb << 8) | lb);
    uint8_t lba = memory->readFromMemory(tempAddress);
    uint8_t hba = memory->readFromMemory(++tempAddress);
    return (uint16_t) ((hba << 8) | lba);

}

uint8_t CPU::getIndirectValueWithRegisterOffset(uint16_t address, uint8_t &reg)
{
    uint8_t zeroPageAddress = memory->readFromMemory(address)+reg;
    uint8_t lb = memory->readFromMemory(zeroPageAddress);
    uint8_t hb = memory->readFromMemory(++zeroPageAddress);
    cyclesCounter += 4;
    return read((uint16_t) ((hb << 8) | lb));

}

uint16_t CPU::getIndirectAddressWithRegisterOffsetIndX(uint16_t address, uint8_t &reg)
{
    uint8_t zeroPageAddress = memory->readFromMemory(address)+reg;
    uint8_t lb = memory->readFromMemory(zeroPageAddress);
    uint8_t hb = memory->readFromMemory(++zeroPageAddress);
    cyclesCounter += 4;
    ++pc;
    return (uint16_t) ((hb << 8) | lb);
}

uint16_t CPU::getIndirectAddressWithRegisterOffsetIndY(uint16_t address, uint8_t &reg)
{
    uint8_t zeroPageAddress = memory->readFromMemory(address);
    uint8_t lb = memory->readFromMemory(zeroPageAddress);
    uint8_t hb = memory->readFromMemory(++zeroPageAddress);
    cyclesCounter += 4;
    ++pc;
    return (uint16_t) ((hb << 8) | lb) + reg;
}

uint8_t CPU::getIndirectValueFromAddressWithRegisterOffset(uint16_t address, uint8_t &reg)
{
    uint8_t zeroPageAddress = memory->readFromMemory(address);
    uint8_t lb = memory->readFromMemory(zeroPageAddress);
    uint8_t hb = memory->readFromMemory(++zeroPageAddress);
    cyclesCounter+=3;
    uint16_t tempAddress = (uint16_t) ((hb << 8) | lb) + reg;

    if((tempAddress & 0xFF00) != (pc & 0xFF00))
                cyclesCounter++;

    return read(tempAddress);
}

void CPU::bitwiseAndWithRegister(uint8_t &reg, uint8_t value)
{    
    uint8_t regValue = reg;
    reg = regValue & value;
    setFlag(fN, reg & 0x80, false);
    setFlag(fZ, reg==0, false);
    ++cyclesCounter;
}

void CPU::arithmeticShiftLeftRegister(uint8_t &reg)
{
    uint16_t value = (uint16_t) reg << 1;

    reg = reg << 1;
    setFlag(fC, (value & 0xFF00) > 0, false);
    setFlag(fZ, (value & 0x00FF) == 0x00, false);
    setFlag(fN, value & 0x80, false);

    cyclesCounter+=2;

}

void CPU::arithmeticShiftLeft(uint16_t address)
{
    uint16_t value = (uint16_t) memory->readFromMemory(address) << 1;

    setFlag(fC, (value & 0xFF00) > 0, false);
    setFlag(fZ, (value & 0x00FF) == 0x00, false);
    setFlag(fN, value & 0x80, false);

    ++cyclesCounter;
    store(address, value);

}

void CPU::bitwiseAndWithAccumulator(uint8_t value)
{
    uint16_t temp = a & value;
    setFlag(fZ, (temp & 0x00FF) == 0x00);
    setFlag(fN, value & (1 << 7));
    setFlag(fV, value & (1 << 6));
    ++cyclesCounter;
}

void CPU::brkInterrupt()
{

    setFlag(fI, true, false);
    store(0x0100 + s, (pc >> 8) & 0x00FF);
    s--;
    store(0x0100 + s, pc & 0x00FF);
    s--;

    setFlag(fB, true, false);
    store(0x0100 + s, getStatusRegister());
    s--;
    setFlag(fB, false, false);

    pc = (uint16_t)read(0xFFFE) | ((uint16_t)read(0xFFFF) << 8);

}

void CPU::nmiInterrupt()
{

    setFlag(fI, true, false);
    --pc;
    store(0x0100 + s, (pc >> 8) & 0x00FF);
    s--;
    store(0x0100 + s, pc & 0x00FF);
    s--;

    setFlag(fB, true, false);
    store(0x0100 + s, getStatusRegister());
    s--;
    setFlag(fB, false, false);

    uint16_t addr_abs = 0xFFFA;    
    uint16_t lo = memory->readFromMemory(addr_abs + 0);
    uint16_t hi = memory->readFromMemory(addr_abs + 1);
    pc = (hi << 8) | lo;

    inInterrupt = true;

}

void CPU::compareWithRegister(uint8_t &reg, uint8_t value)
{
    uint16_t temp = (uint16_t)reg - (uint16_t)value;
    setFlag(fC, reg >= value, false);
    setFlag(fZ, (temp & 0x00FF) == 0, false);
    setFlag(fN, temp & 0x0080, false);
    ++cyclesCounter;
}

void CPU::decrementMemory(uint16_t address)
{
    uint8_t value = memory->readFromMemory(address);
    --value;
    store(address, value);
    setFlag(fZ, (value & 0x00FF) == 0x0000, false);
    setFlag(fN, value & 0x0080, false);
    ++cyclesCounter;
}

void CPU::bitwiseXorWithAccumulator(uint8_t value)
{
    a = a ^ value;
    setFlag(fZ, a == 0x00, false);
    setFlag(fN, a & 0x80, false);
    ++cyclesCounter;

}

void CPU::bitwiseOrWithAccumulator(uint8_t value)
{
    a = a | value;
    setFlag(fZ, a == 0x00, false);
    setFlag(fN, a & 0x80, false);
    ++cyclesCounter;

}

void CPU::incrementMemory(uint16_t address)
{
    uint8_t temp = memory->readFromMemory(address);
    ++temp;
    store(address, temp);
    setFlag(fZ, (temp & 0x00FF) == 0x0000, false);
    setFlag(fN, temp & 0x0080, false);
    cyclesCounter+=4;
}

void CPU::logicalShiftRightAccumulator()
{
    uint8_t value = a;
    uint16_t temp = value >> 1;
    setFlag(fC, value & 0x0001);
    setFlag(fZ, (temp & 0x00FF) == 0x0000);
    setFlag(fN, temp & 0x0080);
    a = a >> 1;
    cyclesCounter += 2;

}

void CPU::logicalShiftRight(uint16_t address)
{
    uint8_t value = memory->readFromMemory(address);
    uint16_t temp = value >> 1;
    setFlag(fC, value & 0x0001, false);
    setFlag(fZ, (temp & 0x00FF) == 0x0000, false);
    setFlag(fN, temp & 0x0080, false);
    store(address, temp);
    ++cyclesCounter;
}

void CPU::rotateLeftAccumulator()
{
    uint16_t temp = (uint16_t)(a << 1) | fC;
    setFlag(fC, temp & 0xFF00, false);
    setFlag(fZ, (temp & 0x00FF) == 0x0000, false);
    setFlag(fN, temp & 0x0080, false);

    a = temp & 0x00FF;
    cyclesCounter += 2;

}

void CPU::rotateLeft(uint16_t address)
{

    uint16_t temp = (uint16_t)(read(address) << 1) | fC;
    setFlag(fC, temp & 0xFF00, false);
    setFlag(fZ, (temp & 0x00FF) == 0x0000, false);
    setFlag(fN, temp & 0x0080, false);
    store(address, temp & 0x00FF);
    ++cyclesCounter;

}

void CPU::rotateRightAccumulator()
{
    uint16_t temp = (uint16_t)(fC << 7) | (a >> 1);
    setFlag(fC, a & 0x01, false);
    setFlag(fZ, (temp & 0x00FF) == 0x0000, false);
    setFlag(fN, temp & 0x0080, false);

    a = temp & 0x00FF;
    cyclesCounter += 2;

}

void CPU::rotateRight(uint16_t address)
{
    uint8_t value = memory->readFromMemory(address);
    uint16_t temp = (uint16_t)(fC << 7) | (value >> 1);
    setFlag(fC, value & 0x01, false);
    setFlag(fZ, (temp & 0x00FF) == 0x0000, false);
    setFlag(fN, temp & 0x0080, false);
    store(address, temp & 0x00FF);
    cyclesCounter += 3;
}

void CPU::returnFromInterrupt()
{
    s++;
    restoreStatusRegister(read(0x0100 + s));
    fB = false;
    s++;
    pc = (uint16_t)memory->readFromMemory(0x0100 + s);
    s++;
    pc |= (uint16_t)memory->readFromMemory(0x0100 + s) << 8;
    cyclesCounter += 3;

    inInterrupt = false;

    interrupt->setNmiOccurred(false);

}

void CPU::returnFromSubroutine()
{
    s++;
    pc = (uint16_t)memory->readFromMemory(0x0100 + s);
    s++;
    pc |= (uint16_t)memory->readFromMemory(0x0100 + s) << 8;

    pc++;

    cyclesCounter += 4;
}


uint16_t CPU::step()
{
    uint8_t opcode = memory->readFromMemory(pc);
    pc++;

    cyclesCounter = 0;

    if(interrupt->isNmiOccurred() && !inInterrupt)
    {
        nmiInterrupt();
        opcode = memory->readFromMemory(pc);
        pc++;
        //interrupt.setNmiOccurred(false);
    }


    switch(opcode)
    {

        //ADC
        case 0x69:      //ADC IMM
            addToRegister(a, read(pc));
            break;

        case 0x65:      //ADC ZP
            addToRegister(a, getValueFromZeroPage(pc));
            break;
        case 0x75:      //ADC ZPX
            addToRegister(a, getValueFromZeroPageWithRegisterOffset(pc, x));
            break;
        case 0x6D:      //ADC ABS
            {
                uint16_t absAddress = getAbsoluteAddress(pc);
                addToRegister(a, memory->readFromMemory(absAddress));
                break;
            }
        case 0x7D:      //ADC ABSX
            {
                uint16_t absAddress = getAbsoluteAddressWithRegisterOffset(pc, x);
                addToRegister(a, memory->readFromMemory(absAddress));
                break;
            }

        case 0x79:      //ADC ABSY
            {
                uint16_t absAddress = getAbsoluteAddressWithRegisterOffset(pc, y);
                addToRegister(a, memory->readFromMemory(absAddress));
                break;
            }
        case 0x61:      //ADC INDX
            addToRegister(a, getIndirectValueWithRegisterOffset(pc, x));
            break;
        case 0x71:      //ADC INDY
            addToRegister(a, getIndirectValueFromAddressWithRegisterOffset(pc, y));
            break;
        //AND
        case 0x29:      //AND IMM
            bitwiseAndWithRegister(a, read(pc));
            break;
        case 0x25:      //AND ZP
            bitwiseAndWithRegister(a, getValueFromZeroPage(pc));
            break;
        case 0x35:      //AND ZPX
            bitwiseAndWithRegister(a, getValueFromZeroPageWithRegisterOffset(pc, x));
            break;
        case 0x2D:      //AND ABS
            {
                uint16_t absAddress = getAbsoluteAddress(pc);
                bitwiseAndWithRegister(a, memory->readFromMemory(absAddress));
                break;
            }
        case 0x3D:      //AND ABSX
            {
                uint16_t absAddress = getAbsoluteAddressWithRegisterOffset(pc, x);
                bitwiseAndWithRegister(a, memory->readFromMemory(absAddress));
                break;
            }
        case 0x39:      //AND ABSY
            {
                uint16_t absAddress = getAbsoluteAddressWithRegisterOffset(pc, y);
                bitwiseAndWithRegister(a, memory->readFromMemory(absAddress));
                break;
            }
        case 0x21:      //AND INDX
            bitwiseAndWithRegister(a, getIndirectValueWithRegisterOffset(pc, x));
            break;
        case 0x31:      //AND INDY
            bitwiseAndWithRegister(a, getIndirectValueFromAddressWithRegisterOffset(pc, y));
            break;
        //ASL
        case 0x0A:      //ASL A
            arithmeticShiftLeftRegister(a);
            break;
        case 0x06:      //ASL ZP
            arithmeticShiftLeft(read(pc));
            break;
        case 0x16:      //ASL ZPX
            arithmeticShiftLeft(getAddressInZeroPageWithRegisterOffset(pc, x));
            break;            
        case 0x0E:      //ASL ABS
            {
                uint16_t absAddress = getAbsoluteAddress(pc);

                arithmeticShiftLeft(absAddress);
                break;
            }
        case 0x1E:      //ASL ABSX
            {
                uint16_t absAddress = getAbsoluteAddressWithRegisterOffset(pc, x);
                arithmeticShiftLeft(memory->readFromMemory(absAddress));
                break;
            }

        //BIT
        case 0x24:     //BIT ZP
            bitwiseAndWithAccumulator(getValueFromZeroPage(pc));
            break;
        case 0x2C:    //BIT ABS
            {
                uint16_t absAddress = getAbsoluteAddress(pc);
                bitwiseAndWithAccumulator(memory->readFromMemory(absAddress));
                break;
            }


        //BRANCH
        case 0x10:      //BPL
            cyclesCounter +=2;
            if(!fN)
            {
                branch(memory->readFromMemory(pc));
            }
            else ++pc;
            break;
        case 0x30:      //BMI
            cyclesCounter +=2;
            if(fN)
            {
                branch(memory->readFromMemory(pc));
            }
            else ++pc;
            break;
        case 0x50:      //BVC
            cyclesCounter +=2;
            if(!fV)
            {
                branch(memory->readFromMemory(pc));
            }
            else ++pc;
            break;
        case 0x70:      //BVS
            cyclesCounter +=2;
            if(fV)
            {
                branch(memory->readFromMemory(pc));
            }
            else ++pc;
            break;
        case 0x90:      //BCC
            cyclesCounter +=2;
            if(!fC)
            {
                branch(memory->readFromMemory(pc));
            }
            else ++pc;
            break;
        case 0xB0:        //BCS
            cyclesCounter +=2;
            if(fC)
            {
                branch(memory->readFromMemory(pc));
            }
            else ++pc;
            break;
        case 0xD0:      //BNE
            cyclesCounter +=2;
            if(!fZ)
            {
                branch(memory->readFromMemory(pc));
            }
            else ++pc;
            break;
        case 0xF0:      //BEQ
            cyclesCounter +=2;
            if(fZ)
            {
                branch(memory->readFromMemory(pc));
            }
            else ++pc;
            break;

        case 0x00:  //BRK
            brkInterrupt();
            break;

        //CMP
        case 0xC9:  //CMP IMM
            compareWithRegister(a, read(pc));
            break;
        case 0xC5:  //CMP ZP
            compareWithRegister(a, getValueFromZeroPage(pc));
            break;
        case 0xD5:  //CMP ZPX
            compareWithRegister(a, getValueFromZeroPageWithRegisterOffset(pc, x));
            break;
        case 0xCD:  //CMP ABS
            compareWithRegister(a, memory->readFromMemory(getAbsoluteAddress(pc)));
            break;
        case 0xDD:  //CMP ABSX
            compareWithRegister(a, memory->readFromMemory(getAbsoluteAddressWithRegisterOffset(pc, x)));
            break;
        case 0xD9:  //CMP ABSY
            compareWithRegister(a, memory->readFromMemory(getAbsoluteAddressWithRegisterOffset(pc, y)));
            break;
        case 0xC1:  //CMP INDX
            compareWithRegister(a, getIndirectValueWithRegisterOffset(pc, x));
            break;
        case 0xD1:  //CMP INDY
            compareWithRegister(a, getIndirectValueFromAddressWithRegisterOffset(pc, y));
            break;

        //CPX
        case 0xE0:  //CPX IMM
            compareWithRegister(x, read(pc));
            break;
        case 0xE4:  //CPX ZP
            compareWithRegister(x, getValueFromZeroPage(pc));
            break;
        case 0xEC:  //CPX ABS
            compareWithRegister(x, memory->readFromMemory(getAbsoluteAddress(pc)));
            break;

        //CPY
        case 0xC0:  //CPY IMM
             compareWithRegister(y, read(pc));
             break;
        case 0xC4:  //CPY ZP
            compareWithRegister(y, getValueFromZeroPage(pc));
            break;
        case 0xCC:  //CPY ABS
            compareWithRegister(y, memory->readFromMemory(getAbsoluteAddress(pc)));
            break;

        //DEC
        case 0xC6:  //DEC ZP
            decrementMemory(read(pc));
            break;
        case 0xD6:  //DEC ZPX
            decrementMemory(getAddressInZeroPageWithRegisterOffset(pc, x));
            break;
        case 0xCE:  //DEC ABS
            decrementMemory(getAbsoluteAddress(pc));
            break;
        case 0xDE:  //DEC ABSX
            decrementMemory(getAbsoluteAddressWithRegisterOffset(pc, x));
            break;

        //EOR
        case 0x49:  //EOR IMM
            bitwiseXorWithAccumulator(read(pc));
            break;
        case 0x45:  //EOR ZP
            bitwiseXorWithAccumulator(getValueFromZeroPage(pc));
            break;
        case 0x55:  //EOR ZPX
            bitwiseXorWithAccumulator(getValueFromZeroPageWithRegisterOffset(pc, x));
            break;
        case 0x4D:  //EOR ABS
            bitwiseXorWithAccumulator(memory->readFromMemory(getAbsoluteAddress(pc)));
            break;
        case 0x5D:  //EOR ABSX
            bitwiseXorWithAccumulator(memory->readFromMemory(getAbsoluteAddressWithRegisterOffset(pc, x)));
            break;
        case 0x59:  //EOR ABSY
            bitwiseXorWithAccumulator(memory->readFromMemory(getAbsoluteAddressWithRegisterOffset(pc, y)));
            break;
        case 0x41:  //EOR INDX
            bitwiseXorWithAccumulator(read(getIndirectValueWithRegisterOffset(pc, x)));
            break;
        case 0x51:  //EOR INDY
            bitwiseXorWithAccumulator(getIndirectValueFromAddressWithRegisterOffset(pc, y));
            break;

        //FLAG SET INSTRUCTIONS
        case 0x18:      //CLC
            setFlag(fC, false);
            break;
        case 0x38:      //SEC
            setFlag(fC, true);
            break;
        case 0x58:       //CLI
            setFlag(fI, false);
            break;
        case 0x78:      //SEI
            setFlag(fI, true);
            break;
        case 0xB8:      //CLV
            setFlag(fV, false);
            break;
        case 0xD8:      //CLD
            setFlag(fD, false);
            break;
        case 0xF8:      //SED
            setFlag(fD, true);
            break;

        //INC
        case 0xE6:      //INC ZP
            incrementMemory(read(pc));
            break;
        case 0xF6:     //INC ZPX
            incrementMemory(getAddressInZeroPageWithRegisterOffset(pc, x));
            break;
        case 0xEE:      //INC ABS
            incrementMemory(getAbsoluteAddress(pc));
            break;
        case 0xFE:     //INC ABSX
            incrementMemory(getAbsoluteAddressWithRegisterOffset(pc, x));
            break;

        //JMP
        case 0x4C:      //JMP ABS
            jump(getAbsoluteAddress(pc));
            cyclesCounter += 1;
            break;

        case 0x6C:      //JMP IND
            jump(getIndirectAddress(pc));
            cyclesCounter += 2;
            break;

        //JSR
        case 0x20:
            jumpToSubroutine(pc);
            break;

        //LDA
        case 0xA9:      //LDA IMM
            loadIntoRegister(a, read(pc));
            break;
        case 0xA5:      //LDA ZP
            loadIntoRegister(a, getValueFromZeroPage(pc));
            break;
        case 0xB5:      //LDA ZPX
            loadIntoRegister(a, getValueFromZeroPageWithRegisterOffset(pc, x));
            break;
        case 0xAD:      //LDA ABS
            loadIntoRegister(a, memory->readFromMemory(getAbsoluteAddress(pc)));
            break;
        case 0xBD:      //LDA ABSX
            loadIntoRegister(a, memory->readFromMemory(getAbsoluteAddressWithRegisterOffset(pc, x)));
            break;
        case 0xB9:      //LDA ABSY
            loadIntoRegister(a, memory->readFromMemory(getAbsoluteAddressWithRegisterOffset(pc, y)));
            break;
        case 0xA1:      //LDA INDX
            loadIntoRegister(a, getIndirectValueWithRegisterOffset(pc, x));
            break;
        case 0xB1:      //LDA INDY
            loadIntoRegister(a, getIndirectValueFromAddressWithRegisterOffset(pc, y));
            break;

        //LDX
        case 0xA2:      //LDX IMM
            loadIntoRegister(x, read(pc));
            break;
        case 0xA6:      //LDX ZP
            loadIntoRegister(x, getValueFromZeroPage(pc));
            break;
        case 0xB6:      //LDX ZPY
            loadIntoRegister(x, getValueFromZeroPageWithRegisterOffset(pc, y));
            break;
        case 0xAE:      //LDX ABS
            loadIntoRegister(x, memory->readFromMemory(getAbsoluteAddress(pc)));
            break;
        case 0xBE:      //LDX ABSY
            loadIntoRegister(x, memory->readFromMemory(getAbsoluteAddressWithRegisterOffset(pc, y)));
            break;

        //LDY
        case 0xA0:      //LDY IMM
            loadIntoRegister(y, read(pc));
            break;
        case 0xA4:      //LDY ZP
            loadIntoRegister(y, getValueFromZeroPage(pc));
            break;
        case 0xB4:      //LDY ZPX
            loadIntoRegister(y, getValueFromZeroPageWithRegisterOffset(pc, x));
            break;
        case 0xAC:      //LDY ABS
            loadIntoRegister(y, memory->readFromMemory(getAbsoluteAddress(pc)));
            break;
        case 0xBC:      //LDY ABSX
            loadIntoRegister(y, memory->readFromMemory(getAbsoluteAddressWithRegisterOffset(pc, x)));
            break;

        //LSR
        case 0x4A:      //LSR A
            logicalShiftRightAccumulator();
            break;
        case 0x46:      //LSR ZP
            logicalShiftRight(read(pc));
            break;
        case 0x56:      //LSR ZPX
            logicalShiftRight(getAddressInZeroPageWithRegisterOffset(pc, x));
            break;
        case 0x4E:      //LSR ABS
            logicalShiftRight(getAbsoluteAddress(pc));
            break;
        case 0x5E:      //LSR ABSX
            logicalShiftRight(memory->readFromMemory(getAbsoluteAddressWithRegisterOffset(pc, x)));
            break;

        //NOP
        case 0xEA:
            cyclesCounter+=2;
            break;

        //ORA
        case 0x09:      //ORA IMM
            bitwiseOrWithAccumulator(read(pc));
            break;
        case 0x05:      //ORA ZP
            bitwiseOrWithAccumulator(getValueFromZeroPage(pc));
            break;
        case 0x15:      //ORA ZPX
            bitwiseOrWithAccumulator(getValueFromZeroPageWithRegisterOffset(pc, x));
            break;
        case 0x0D:      //ORA ABS
            bitwiseOrWithAccumulator(memory->readFromMemory(getAbsoluteAddress(pc)));
            break;
        case 0x1D:      //ORA ABSX
            bitwiseOrWithAccumulator(memory->readFromMemory(getAbsoluteAddressWithRegisterOffset(pc, x)));
            break;
        case 0x19:      //ORA ABSY
            bitwiseOrWithAccumulator(memory->readFromMemory(getAbsoluteAddressWithRegisterOffset(pc, y)));
            break;
        case 0x01:      //ORA INDX
            bitwiseOrWithAccumulator(getIndirectValueWithRegisterOffset(pc, x));
            break;
        case 0x11:      //ORA INDY
            bitwiseOrWithAccumulator(getIndirectValueFromAddressWithRegisterOffset(pc, y));
            break;

        //Register Instructions
        case 0xAA:      //TAX
            loadIntoRegister(x, a);
            ++cyclesCounter;
            break;
        case 0x8A:      //TXA
            loadIntoRegister(a, x);
            ++cyclesCounter;
            break;
        case 0xCA:      //DEX
            decrementRegisterValue(x);
            break;
        case 0xE8:      //INX
            incrementRegisterValue(x);
            break;
        case 0xA8:      //TAY
            loadIntoRegister(y, a);
            ++cyclesCounter;
            break;
        case 0x98:      //TYA
            loadIntoRegister(a, y);
            ++cyclesCounter;
            break;
        case 0x88:      //DEY
            decrementRegisterValue(y);
            break;
        case 0xC8:      //INY
            incrementRegisterValue(y);
            break;

        //ROL
        case 0x2A:      //ROL A
            rotateLeftAccumulator();
            break;
        case 0x26:      //ROL ZP
            rotateLeft(memory->readFromMemory(pc));
            break;
        case 0x36:      //ROL ZPX
            rotateLeft(getAddressInZeroPageWithRegisterOffset(pc, x));
            break;
        case 0x2E:      //ROL ABS
            rotateLeft(getAbsoluteAddress(pc));
            pc--;
            break;
        case 0x3E:      //ROL ABSX
            rotateLeft(memory->readFromMemory(getAbsoluteAddressWithRegisterOffset(pc, x)));
            break;

        //ROR
        case 0x6A:      //ROR A
            rotateRightAccumulator();
            break;
        case 0x66:      //ROR ZP
            rotateRight(read(pc));
            break;
        case 0x76:      //ROR ZPX
            rotateRight(getAddressInZeroPageWithRegisterOffset(pc, x));
            break;
        case 0x6E:      //ROR ABS
            rotateRight(getAbsoluteAddress(pc));
            break;
        case 0x7E:      //ROR ABSX
            rotateRight(memory->readFromMemory(getAbsoluteAddressWithRegisterOffset(pc, x)));
            break;

        //RTI
        case 0x40:      //RTI
            returnFromInterrupt();
            break;

        //RTS
        case 0x60:      //RTS
            returnFromSubroutine();
            break;

        //SBC
        case 0xE9:      //SBC IMM
            substractFromRegister(a, read(pc));
            break;
        case 0xE5:      //SBC ZP
            substractFromRegister(a, getValueFromZeroPage(pc));
            break;
        case 0xF5:      //SBC ZPX
            substractFromRegister(a, getValueFromZeroPageWithRegisterOffset(pc, x));
            break;
        case 0xED:      //SBC ABS
            substractFromRegister(a, memory->readFromMemory(getAbsoluteAddress(pc)));
            break;
        case 0xFD:      //SBC ABSX
            substractFromRegister(a, memory->readFromMemory(getAbsoluteAddressWithRegisterOffset(pc, x)));
            break;
        case 0xF9:      //SBC ABSY
            substractFromRegister(a, memory->readFromMemory(getAbsoluteAddressWithRegisterOffset(pc, y)));
            break;
        case 0xE1:      //SBC INDX
            substractFromRegister(a, getIndirectValueWithRegisterOffset(pc, x));
            break;
        case 0xF1:      //SBC INDY
            substractFromRegister(a, getIndirectValueFromAddressWithRegisterOffset(pc, y));
            break;

        //STA
        case 0x85:      //STA ZP
            store(read(pc), a);
            break;
        case 0x95:      //STA ZPX
            store(getAddressInZeroPageWithRegisterOffset(pc, x), a);
            break;
        case 0x8D:      //STA ABS
            store(getAbsoluteAddress(pc), a);
            break;
        case 0x9D:      //STA ABSX
            store(getAbsoluteAddressWithRegisterOffset(pc, x), a);
            break;
        case 0x99:      //STA ABSY
            store(getAbsoluteAddressWithRegisterOffset(pc, y), a);
            break;
        case 0x81:      //STA INDX
            store(getIndirectAddressWithRegisterOffsetIndX(pc, x), a);
            break;
        case 0x91:      //STA INDY
            store(getIndirectAddressWithRegisterOffsetIndY(pc, y), a);
            break;


        //Stack Instructions
        case 0x9A:      //TXS
            s = x; //flags not affected
            ++cyclesCounter;
            break;      
        case 0xBA:      //TSX
            loadIntoRegister(x, s);
            setFlag(fZ, x == 0x00, false);
            setFlag(fN, x & 0x80, false);
            ++cyclesCounter;
            break;
       case 0x48:       //PHA
            store(0x0100 + s, a);
            s--;
            cyclesCounter+=2;
            break;
       case 0x68:       //PLA
            s++;
            a = memory->readFromMemory(0x0100 + s);
            setFlag(fZ, a == 0x00, false);
            setFlag(fN, a & 0x0080, false);
            cyclesCounter+=3;
            break;
       case 0x08: //PHP
            setFlag(fB, true, false);
            store(0x0100 + s, getStatusRegister());
            setFlag(fB, false, false);
            cyclesCounter+=3;
            s--;
            break;
       case 0x28:       //PLP
            s++;
            restoreStatusRegister(memory->readFromMemory(0x0100 + s));
            cyclesCounter+=4;
            break;

        //STX
        case 0x86:      //STX ZP
            store(read(pc), x);
            ++cyclesCounter;
            break;
        case 0x96:      //STX ZPY
            store(read(pc + y), x);
            cyclesCounter+=2;
            break;
        case 0x8E:      //STX ABS
            store(getAbsoluteAddress(pc), x);
            ++cyclesCounter;
            break;

        //STY
        case 0x84:      //STY ZP
            store(read(pc), y);
            ++cyclesCounter;
            break;
        case 0x94:      //STX ZPX
            store((read(pc) + x), y);
            cyclesCounter+=2;
            break;
        case 0x8C:      //STX ABS
            store(getAbsoluteAddress(pc), y);
            ++cyclesCounter;
            break;

        default:
            std::cout<<"Unknown opcode at "<<(std::hex)<<pc<<std::endl;

    }
    //std::cout<<(std::hex)<<pc<<std::endl;

    cpuCycleCounter+=cyclesCounter;
    return cyclesCounter;

}



