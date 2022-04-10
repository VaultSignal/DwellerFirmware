#include "Buzzer.h"

Buzzer::Buzzer(byte BUZZER) {
    this->BUZZER = BUZZER;
    init();
}

void Buzzer::init() {
    pinMode(BUZZER, OUTPUT);
}

void Buzzer::unlockSound() {
  tone(BUZZER, NOTE_E7);
  delay(120);
  tone(BUZZER, NOTE_G7);
  delay(140);
  tone(BUZZER, NOTE_C8);
  delay(300);
  noTone(BUZZER);
}

void Buzzer::lockSound() {
  tone(BUZZER, NOTE_C8);
  delay(150);
  noTone(BUZZER);
  delay(10);
  tone(BUZZER, NOTE_C8);
  delay(150);
  noTone(BUZZER);
}

// Play alarm sound by given duration. Duration must be seconds.
void Buzzer::alarmSound(int duration) {
  // Every one loop takes aproximately 100 milliseconds.
  for(int i = 0; i < duration * 10; i++) {
    tone(BUZZER, NOTE_E5);
    delay(40);
    tone(BUZZER, NOTE_C8);
    delay(60);
  }
  noTone(BUZZER);
}
