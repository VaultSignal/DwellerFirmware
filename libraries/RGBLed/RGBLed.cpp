#include "RGBLed.h"

RGBLed::RGBLed(byte RED, byte GREEN, byte BLUE) {
    this->RED = RED;
    this->GREEN = GREEN;
    this->BLUE = BLUE;
    init();
}

void RGBLed::init() {
    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(BLUE, OUTPUT);
}

// Set led color given that R G B values that are between 0 and 255
void RGBLed::ledColor(int r, int g, int b) {
  analogWrite(RED, 255 - r);
  analogWrite(GREEN, 255 - g);
  analogWrite(BLUE, 255 - b);
}
