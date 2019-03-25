
// Libraries
#include <Wire.h>
#include <Rotary.h>

#include "Screen.h"
#include "Fan.h"
#include "Speaker.h"
#include "Temperature.h"

// Config
#define SERIAL_BAUD_RATE        9600
#define I2C_SLAVE_ADDRESS       0x04
#define TEMP_ALERT_LEVEL        35

// Pins
#define PIN_DISPLAY_CLK         11
#define PIN_DISPLAY_DIO         10

#define PIN_FAN_PWM             3
#define PIN_FAN_RPM             2

#define PIN_TMP_0               A0
#define PIN_TMP_1               A1
#define PIN_TMP_2               A2
#define PIN_TMP_3               A3

#define PIN_PIEZO_BUZZER        8

#define PIN_ROTARY_A            6
#define PIN_ROTARY_B            5
#define PIN_ROTARY_SW           4

// Globals
// int currentTempSensor = 0;
// int currentFanSpeed = 0;
// int highestTempSensor = 0;
// int highestTempValue = 0;
// bool highTemperature = false;

// int numTempSensors = 4;
// float temperatures[4] = {0, 0, 0, 0};

int sensorCount = 4;
int sensorPins[4] = {PIN_TMP_0, PIN_TMP_1, PIN_TMP_2, PIN_TMP_3};
bool speedChanged = false;
bool forceTempRefresh = true;
unsigned long previousRotate = 0;
unsigned long previousTempRefresh = 0;
unsigned long previousFanSpeedRefresh = 0;
unsigned long previousFanSpeedChange = 0;
String scommand;

// Modules
Rotary rotary = Rotary(PIN_ROTARY_A, PIN_ROTARY_B);

Fan fan = Fan(PIN_FAN_PWM, PIN_FAN_RPM);
Temperature temperature = Temperature(sensorCount, sensorPins);
Screen screen = Screen(PIN_DISPLAY_CLK, PIN_DISPLAY_DIO);
Speaker speaker = Speaker(PIN_PIEZO_BUZZER);


// Setup
void setup() {
    // Serial
    Serial.begin(SERIAL_BAUD_RATE);
    Serial.println("Initializing...");

    // Setup modules
    fan.setup();
    temperature.setup();
    screen.setup();
    speaker.setup();

    // Init
    pinMode(PIN_ROTARY_SW, INPUT_PULLUP);
    delay(2000);
}

// Loop
void loop() {
    // Timer
    long currentMillis = millis();

    // Serial command
    while(Serial.available()) {
      scommand = Serial.readString();
      if (scommand.substring(0, 7) == "SETFAN="){
        //Serial.println();
        long fanPrc = scommand.substring(7).toInt();
        fan.setSpeedLevel(fanPrc);
      }
    }
    
    //  Rotary control
    // --------------------------------------------------------------------
    unsigned char rotaryDirection = rotary.process();

    bool rotaryButtonClicked = false;
    if (digitalRead(PIN_ROTARY_SW) == LOW) rotaryButtonClicked = true;

    // Control
    if ((rotaryDirection == DIR_CW) || (rotaryDirection == DIR_CCW)) {
        previousRotate = currentMillis;

        // Fan speed controller
        if (!rotaryButtonClicked) {
            // Current speed level
            int speedLevel = fan.getSpeedLevel();

            // Up
            if (rotaryDirection == DIR_CW) {
                speedLevel += 5;
                if (speedLevel > 100) speedLevel = 100;
            }
            // Down
            else if (rotaryDirection == DIR_CCW) {
                speedLevel -= 5;
                if (speedLevel < 0) speedLevel = 0;
            }

            // Set speed level
            Serial.print("Rotary control, fan speed level ");
            Serial.print(speedLevel);

            if (speedLevel != fan.getSpeedLevel()) {
                Serial.println(" (changed)");
                fan.setSpeedLevel(speedLevel);
                screen.showFanSpeedLevel(fan.getSpeedLevel());
                speedChanged = true;
            } else {
                Serial.println(" (NOT changed)");
                screen.showFanSpeedLevel(fan.getSpeedLevel());
                delay(500);
                speedChanged = true;

                if (currentMillis - previousFanSpeedChange > 3000) {
                    speedChanged = false;
                    forceTempRefresh = true;
                }
            }
            
            previousFanSpeedChange = currentMillis;
        }
    }
    else {
        // Cycle all sensor temperatures
        if (rotaryButtonClicked) {

            speaker.beep();
            
            for (int i = 0; i < temperature.sensorCount; i++) {
                screen.showTemperature(temperature.read(i), (i + 1));
                delay(1500);
            }
            screen.showTemperature(temperature.highestTemperature, -1);
            delay(500);
        }
    }

    //  State machine(?)
    // --------------------------------------------------------------------
    if (speedChanged && (currentMillis - previousRotate > 1000) && !rotaryButtonClicked) {
        // Show fan speed after level change
        if (currentMillis - previousFanSpeedChange <= 5000) {
            if (currentMillis - previousFanSpeedRefresh >= 250) {
                // Update display
                screen.showFanSpeed(fan.getSpeed());
                
                previousFanSpeedRefresh = currentMillis;
            }
        }
    }

    // Refresh all temperatures
    if (forceTempRefresh || ((currentMillis - previousTempRefresh >= 10000) && 
        (currentMillis - previousFanSpeedChange > 5000))) {
        
        temperature.readAll();

        screen.showTemperature(temperature.highestTemperature, -1);

        forceTempRefresh = false;
        previousTempRefresh = currentMillis;
    }
}
