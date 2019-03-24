#ifndef Temperature_h
#define Temperature_h

#include "Arduino.h"
#include "Temperature.h"

class Temperature
{
  public:
    Temperature(int sensorCount, int sensorPins[]);
    void setup();
    float read(int sensor);
    void readAll();

    int sensorCount;
    float highestTemperature;

  private:
    int *sensorPins;
};

#endif