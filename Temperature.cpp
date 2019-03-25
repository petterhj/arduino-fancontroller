#include "Temperature.h"

// ===============================================================
//  Temperature
// ===============================================================
Temperature::Temperature(int sensorCount, int sensorPins[]) {
    this->sensorCount = sensorCount;
    this->sensorPins = sensorPins;
    this->highestTemperature = 0;
}

// Setup
void Temperature::setup() {
    for (int i = 0; i < this->sensorCount; i++) {
        pinMode(this->sensorPins[i], INPUT);
    }
}

// Read
float Temperature::read(int sensor) {
    //Serial.print("Temperature sensor ");
    //Serial.print(sensor);
    //Serial.print(" (pin ");
    //Serial.print(this->sensorPins[sensor]);
    //Serial.print("): ");

    int raw_adc = analogRead(this->sensorPins[sensor]);
    long resistance = 10000*((1024.0 / raw_adc) - 1); 
    float temp = log(resistance); // Saving the Log(resistance) so not to calculate  it 4 times later
    temp = 1 / (0.001129148 + (0.000234125 * temp) + (0.0000000876741 * temp * temp * temp));
    temp = temp - 273.15;  // Convert Kelvin to Celsius                      
    if (isnan(temp) || temp < 0) {
        temp = 0;
    }

    if (temp > this->highestTemperature) {
        this->highestTemperature = temp;
    }

    //Serial.println(temp);

    return temp;
}

// Read all
void Temperature::readAll() {
    Serial.println("Refreshing all temperature sensors:");
        
    this->highestTemperature = 0;
    
    Serial.print("##TMP#");
    
    for (int i = 0; i < this->sensorCount; i++) {
        float tmp = this->read(i);
        Serial.print(i);
        Serial.print("=");
        Serial.print(tmp);
        Serial.print("#");
    }

    Serial.println();
    
    Serial.print("Highest temperature: ");
    Serial.println(this->highestTemperature);
}
