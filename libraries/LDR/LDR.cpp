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

// Read LDR values then check if there is a value bigger than the threshold.
bool LDR::isTriggered()
{
    this->ldr0_value = analogRead(LDR0);
    this->ldr1_value = analogRead(LDR1);
    this->ldr2_value = analogRead(LDR2);

    if (this->ldr0_value > sensitivity || this->ldr1_value > sensitivity || this->ldr2_value > sensitivity)
    {
        return true;
    }
    return false;
}