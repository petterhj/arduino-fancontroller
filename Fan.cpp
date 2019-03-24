#include "Fan.h"

// ===============================================================
//  Fan
// ===============================================================
Fan::Fan(int fanPwmPin, int fanRpmPin) {
    this->fanPwmPin = fanPwmPin;
    this->fanRpmPin = fanRpmPin;
    this->currentSpeedLevel = 0;
}

// Setup
void Fan::setup() {
    pinMode(this->fanRpmPin, INPUT_PULLUP);

    this->setSpeedLevel(10);
}

// Process
void Fan::process() {
    
}

// Set fan speed level
int Fan::setSpeedLevel(int percent) {
    if (percent >= 0 && percent <= 100) {
        Serial.print("Setting fan speed at ");
        Serial.print(percent);
        Serial.print(" %, pwm ");
    
        int pwm = ((percent * 255) / 100);

        analogWrite(this->fanPwmPin, pwm);

        this->currentSpeedLevel = percent;

        Serial.println(pwm);
    } else {
        Serial.println("Invalid fan speed (range 0-100)");
    }
}

// Get fan speed
int Fan::getSpeedLevel() {
    return this->currentSpeedLevel;
}

// Get fan RPM
int Fan::getSpeed() {
    Serial.print("Current fan speed: ");

    long pulseDuration = pulseIn(this->fanRpmPin, LOW);
    double frequency = 1000000 / pulseDuration;
    
    int rpm = 0;
    
    if (pulseDuration > 0) {
        rpm = (frequency / 2 * 60 / 2);
    }

    Serial.print(rpm);
    Serial.println(" rpm");
    
    return rpm;
}

