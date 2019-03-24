#include "Screen.h"

// ===============================================================
//  Screen
// ===============================================================
Screen::Screen(int screenClkPin, int screenDioPin) {
    this->screenClkPin = screenClkPin;
    this->screenDioPin = screenDioPin;

    this->display = new SevenSegmentTM1637(this->screenClkPin, this->screenDioPin);
}

// Setup
void Screen::setup() {
    this->display->begin();
    this->display->setBacklight(100);
    this->display->clear();

    this->display->setCursor(0, 0);
    this->display->print("Init");
}

// Show fan speed level
void Screen::showFanSpeedLevel(int level) {
    this->display->clear();
    this->display->setCursor(0, 0);
    this->display->print("F");
    this->display->setCursor(0, ((level < 100) ? 2 : 1));
    this->display->print(level);
}

// Show fan speed
void Screen::showFanSpeed(int speed) {
    this->display->clear();
    this->display->print(speed);
}

// Show temperature
void Screen::showTemperature(float temperature, int sensor) {
    int col = 0;
    int tmp = round(temperature);

    this->display->setCursor(0, 0);

    if (tmp < 10) {
        this->display->print(" ");
        col = 1;
    }

    this->display->setCursor(0, col);
    this->display->print(tmp);
    
    byte rawData;
    rawData = B11100011;

    this->display->printRaw(rawData, 2);
    this->display->setCursor(0, 3);

    if (sensor > -1) {
        this->display->print(sensor);
    } else {
        this->display->print("C");
    }
}

// Cycle temperature sensors
void Screen::cycleTemperatureSensors() {
    this->display->clear();
    this->display->print("test");    
}