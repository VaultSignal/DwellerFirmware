#ifndef LDR_H
#define LDR_H
#include <Arduino.h>

class LDR
{
private:
    byte LDR0;
    byte LDR1;
    byte LDR2;
    int sensitivity;
    void init();

public:
    int ldr0_value;
    int ldr1_value;
    int ldr2_value;
    LDR(byte, byte, byte, int);
    bool isTriggered();
};

#endif