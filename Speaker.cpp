#include "Speaker.h"

// ===============================================================
//  Speaker
// ===============================================================
Speaker::Speaker(int speakerPin) {
    this->speakerPin = speakerPin;
}

// Setup
void Speaker::setup() {
    pinMode(this->speakerPin, OUTPUT);

    NewTone(this->speakerPin, 1000, 100);
    NewTone(this->speakerPin, 500, 125);
    NewTone(this->speakerPin, 1300, 150);
}

// Beep
void Speaker::beep() {
    NewTone(this->speakerPin, 1000, 100);
}