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

// Read the lid value then check if it is higher than the threshold value.
bool Lid::isTriggered()
{
    int lid = analogRead(LID);
    if (lid < threshHold)
    {
        return true;
    }
    else
    {
        return false;
    }
}