#include "Lid.h"

Lid::Lid(byte LID, int threshHold)
{
    this->LID = LID;
    this->threshHold = threshHold;
}

void Lid::init()
{
    pinMode(LID, INPUT);
}

bool Lid::isTriggered()
{
    lid = analogRead(LID);

    if (lid > threshHold)
    {
        return True;
    }
    else
    {
        return False;
    }
}