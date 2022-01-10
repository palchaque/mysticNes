#include "interrupts.h"

using namespace mysticNes;

void Interrupts::setNmiOccurred(bool value)
{
    nmiOccurred = value;
}

bool Interrupts::isNmiOccurred()
{
    return nmiOccurred;
}
