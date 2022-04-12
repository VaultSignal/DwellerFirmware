#include "Buzzer.h"

Buzzer::Buzzer(byte BUZZER)
{
  this->BUZZER = BUZZER;
  init();
}

void Buzzer::init()
{
  pinMode(BUZZER, OUTPUT);
}

// Unlock sound
void Buzzer::unlockSound()
{
  tone(BUZZER, NOTE_E7);
  delay(120);
  tone(BUZZER, NOTE_G7);
  delay(140);
  tone(BUZZER, NOTE_C8);
  delay(300);
  noTone(BUZZER);
}

// Lock sound
void Buzzer::lockSound()
{
  tone(BUZZER, NOTE_C8);
  delay(150);
  noTone(BUZZER);
  delay(10);
  tone(BUZZER, NOTE_C8);
  delay(150);
  noTone(BUZZER);
}

// Play alarm sound by given duration. Duration must be seconds.
void Buzzer::alarmSound(double duration)
{
  // Every one loop takes aproximately 100 milliseconds.
  for (int i = 0; i < duration * 10; i++)
  {
    tone(BUZZER, NOTE_E5);
    delay(40);
    tone(BUZZER, NOTE_C8);
    delay(60);
  }
  noTone(BUZZER);
}

// Openning music of the dweller
void Buzzer::openningMusic()
{
  int tempo = 220;

  int buzzer = 11;

  int melody[] = {

      NOTE_E5,
      8,
      NOTE_D5,
      8,
      NOTE_FS4,
      4,
      NOTE_GS4,
      4,
      NOTE_CS5,
      8,
      NOTE_B4,
      8,
      NOTE_D4,
      4,
      NOTE_E4,
      4,
      NOTE_B4,
      8,
      NOTE_A4,
      8,
      NOTE_CS4,
      4,
      NOTE_E4,
      4,
      NOTE_A4,
      2,
  };

  int notes = sizeof(melody) / sizeof(melody[0]) / 2;

  int wholenote = (60000 * 4) / tempo;

  int divider = 0, noteDuration = 0;

  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2)
  {
    divider = melody[thisNote + 1];
    if (divider > 0)
    {
      noteDuration = (wholenote) / divider;
    }
    else if (divider < 0)
    {
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5;
    }

    tone(BUZZER, melody[thisNote], noteDuration * 0.9);

    delay(noteDuration);

    noTone(BUZZER);
  }
}