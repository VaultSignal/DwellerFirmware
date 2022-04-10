#ifndef BUZZER_H
#define BUZZER_H
#include <Arduino.h>

#define NOTE_E5  659
#define NOTE_E7  2637
#define NOTE_G7  3136
#define NOTE_C8  4186

class Buzzer { 
    private:
        byte BUZZER;

    public:
        Buzzer(byte);
        void init();
        void unlockSound();
        void lockSound();
        void alarmSound(int);
};

#endif