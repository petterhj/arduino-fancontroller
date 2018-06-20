#ifndef Speaker_h
#define Speaker_h

#include "Arduino.h"
#include "NewTone.h"


class Speaker
{
  public:
    Speaker(int spakerPin);
    void setup();
    void beep();
    
  private:
    int speakerPin;
};

#endif