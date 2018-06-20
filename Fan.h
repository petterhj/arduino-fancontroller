#ifndef Fan_h
#define Fan_h

#include "Arduino.h"


class Fan
{
  public:
    Fan(int fanPwmPin, int fanRpmPin);
    void setup();
    void process();
    int setSpeedLevel(int percent);
    int getSpeedLevel();
    int getSpeed();
    
  private:
    int fanPwmPin;
    int fanRpmPin;
    int currentSpeedLevel;
};

#endif