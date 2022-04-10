#ifndef LID_H
#define LID_H
#include <Arduino.h>

class Lid
{
private:
    byte LID;
    int threshHold;

public:
    Lid(byte, int);
    void init();
    bool isTriggered();
};

#endif