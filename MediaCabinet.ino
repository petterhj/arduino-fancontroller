
// Libraries
#include <Wire.h>
#include <Rotary.h>

#include "Screen.h"
#include "Fan.h"
#include "Speaker.h"

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

// Modules
Rotary rotary = Rotary(PIN_ROTARY_A, PIN_ROTARY_B);

Screen screen = Screen(PIN_DISPLAY_CLK, PIN_DISPLAY_DIO);
Fan fan = Fan(PIN_FAN_PWM, PIN_FAN_RPM);
//Sensors sensors = Sensors(PIN_TMP_0, PIN_TMP_1, PIN_TMP_2, PIN_TMP_3);
Speaker speaker = Speaker(PIN_PIEZO_BUZZER);


// Globals
// int currentTempSensor = 0;
// int currentFanSpeed = 0;
// int highestTempSensor = 0;
// int highestTempValue = 0;
// bool highTemperature = false;

// int numTempSensors = 4;
// float temperatures[4] = {0, 0, 0, 0};

bool speedChanged = false;
bool forceTempRefresh = false;
int previousFanSpeed = 0;
unsigned long previousRotate = 0;
unsigned long previousTempRefresh = 0;
unsigned long previousFanSpeedRefresh = 0;
unsigned long previousFanSpeedChange = 0;


// Setup
void setup() {
    // Serial
    Serial.begin(SERIAL_BAUD_RATE);
    Serial.println("Initializing...");

    // Setup modules
    screen.setup();
    fan.setup();
    speaker.setup();

    // Init
    pinMode(PIN_ROTARY_SW, INPUT_PULLUP);
}

// Loop
void loop() {
    // Timer
    long currentMillis = millis();
        
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
            if (speedLevel != fan.getSpeedLevel()) {
                fan.setSpeedLevel(speedLevel);
                screen.showFanSpeedLevel(fan.getSpeedLevel());
                speedChanged = true;
            } else {
                screen.showFanSpeedLevel(fan.getSpeedLevel());
                delay(300);
                forceTempRefresh = true;
            }
            
            
            previousFanSpeedChange = currentMillis;
        }
    }
    else {
        // Cycle all sensor temperatures
        if (rotaryButtonClicked) {

            speaker.beep();
            // screen.cycleTemperatureSensors();
            screen.showTemperature(10, 1);
            delay(1500);
            screen.showTemperature(15, 2);
            delay(1500);
            screen.showTemperature(20, 3);
            delay(1500);
            screen.showTemperature(25, 4);
            delay(1500);
            screen.showTemperature(25, -1);
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
                int currentSpeed = fan.getSpeed();

                if (previousFanSpeed != currentSpeed) { 
                    screen.showFanSpeed(currentSpeed);
                }

                previousFanSpeed = currentSpeed;
                previousFanSpeedRefresh = currentMillis;
            }
        }
    }

    // Refresh all temperatures
    if (forceTempRefresh || ((currentMillis - previousTempRefresh >= 10000) && 
        (currentMillis - previousFanSpeedChange > 5000))) {
        
        Serial.println("REFRESH all tmp. sensors");
        screen.showTemperature(25, -1);

        forceTempRefresh = false;
        previousFanSpeed = 0;
        previousTempRefresh = currentMillis;
    }
}