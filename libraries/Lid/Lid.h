#ifndef LID_H
#define LID_H
#include <Arduino.h>

class Lid
{
private:
    byte LID;

public:
    Lid(byte);
    void init();
    bool isTriggered();
};

#endif