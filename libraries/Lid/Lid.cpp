#include "Lid.h"

Lid::Lid(byte LID)
{
    this->LID = LID;
}

void Lid::init()
{
    pinMode(LID, INPUT);
}

// Read the lid value then check if it is higher than the threshold value.
bool Lid::isTriggered()
{
    int lid = digitalRead(LID);
    return lid == HIGH;
}