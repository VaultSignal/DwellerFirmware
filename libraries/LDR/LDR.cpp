#include "LDR.h"

LDR::LDR(byte LDR0, byte LDR1, byte LDR2, int sensitivity)
{
    this->LDR0 = LDR0;
    this->LDR1 = LDR1;
    this->LDR2 = LDR2;
    this->sensitivity = sensitivity;
    init();
}

void LDR::init()
{
    pinMode(LDR0, INPUT);
    pinMode(LDR1, INPUT);
    pinMode(LDR2, INPUT);
}

bool LDR::isTriggered()
{
    
    int ldr0 = analogRead(LDR0);
    int ldr1 = analogRead(LDR1);
    int ldr2 = analogRead(LDR2);

    if (ldr0 > sensitivity || ldr1 > sensitivity || ldr2 > sensitivity)
    {
        return true;
    }
    return false;
}