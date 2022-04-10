#ifndef RGBLED_H
#define RGBLED_H
#include <Arduino.h>

class RGBLed { 
    private:
        byte RED;
        byte GREEN;
        byte BLUE;

    public:
        RGBLed(byte, byte, byte);
        void init();
        void ledColor(int, int, int);
};

#endif