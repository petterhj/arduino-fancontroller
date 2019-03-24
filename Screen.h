#ifndef Screen_h
#define Screen_h

#include "Arduino.h"
#include "math.h"
#include "SevenSegmentTM1637.h"


class Screen
{
  public:
    Screen(int screenClkPin, int screenDioPin);
    void setup();
    void showFanSpeedLevel(int level);
    void showFanSpeed(int speed);
    void showTemperature(float temperature, int sensor);
    void cycleTemperatureSensors();

    SevenSegmentTM1637* display;
    
  private:
    int screenClkPin;
    int screenDioPin;
};

#endif