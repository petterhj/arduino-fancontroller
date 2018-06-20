
// Libraries
#include <NewTone.h>
#include <Wire.h>
// #include <TM1637Display.h>
#include <SevenSegmentTM1637.h>
#include <Rotary.h>

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
SevenSegmentTM1637 display(PIN_DISPLAY_CLK, PIN_DISPLAY_DIO);
Rotary rotary = Rotary(PIN_ROTARY_A, PIN_ROTARY_B);

// Globals
int currentTempSensor = 0;
int currentFanSpeed = 0;
int highestTempSensor = 0;
int highestTempValue = 0;
bool highTemperature = false;

int numTempSensors = 4;
float temperatures[4] = {0, 0, 0, 0};

unsigned long previousRotate = 0;
unsigned long previousTempRefresh = 0;
unsigned long previousTempSensorChange = 0;
unsigned long previousFanSpeedRefresh = 0;
unsigned long previousFanSpeedChange = 0;

// Setup
void setup() {
    // Pin modes
    pinMode(PIN_FAN_RPM, INPUT_PULLUP);
    pinMode(PIN_ROTARY_SW, INPUT_PULLUP);
    pinMode(PIN_PIEZO_BUZZER, OUTPUT);

    // Serial
    Serial.begin(SERIAL_BAUD_RATE);

    // Display
    display.begin();
    display.setBacklight(100);
    display.clear();

    // Fan
    setFanSpeed(20);

    // Speaker
    NewTone(PIN_PIEZO_BUZZER, 1000, 100);

    // Ready
    Serial.println("READY");
}

// Loop
void loop() {
    // Timer
    long currentMillis = millis();
    
    // Rotary control
    // ==============================================================================
    unsigned char result = rotary.process();

    bool rotaryButtonClicked = false;
    if (digitalRead(PIN_ROTARY_SW) == LOW) rotaryButtonClicked = true;

    if ((result == DIR_CW) || (result == DIR_CCW)) {
        previousRotate = currentMillis;

        // Fan speed controller
        // ----------------------------------------------------------------------
        if (!rotaryButtonClicked) {
            // Up
            if (result == DIR_CW) {
                Serial.print("ROTARY UP CLICKED, increasing speed to ");
                currentFanSpeed += 5;
                if (currentFanSpeed > 100) currentFanSpeed = 100;
                Serial.print(currentFanSpeed);
                Serial.println(" %");
            }
            // Down
            else if (result == DIR_CCW) {
                Serial.print("ROTARY UP CLICKED, decreasing speed to ");
                currentFanSpeed -= 5;
                if (currentFanSpeed < 0) currentFanSpeed = 0;
                Serial.print(currentFanSpeed);
                Serial.println(" %");
            }

            display.clear();
            display.setCursor(0, 0);
            display.print("F");
            display.setCursor(0, ((currentFanSpeed < 100) ? 2 : 1));
            display.print(currentFanSpeed);

            //display.showNumberDec(currentFanSpeed, false, 4, 0);
            setFanSpeed(currentFanSpeed);
            previousFanSpeedChange = currentMillis;
            previousTempSensorChange = currentMillis;
        }

        // Temperature sensor select
        // ----------------------------------------------------------------------
        else {
            // Up
            if (result == DIR_CW) {
                currentTempSensor++;
                if (currentTempSensor > 3) currentTempSensor = 0;
                Serial.print("ROTARY UP, selecting sensor #");
                Serial.println(currentTempSensor);
            }
            // Down
            else if (result == DIR_CCW) {
                currentTempSensor--;
                if (currentTempSensor < 0) currentTempSensor = 3;
                Serial.print("ROTARY DOWN, selecting sensor #");
                Serial.println(currentTempSensor);
            }

            display.clear();
            display.setCursor(0, 0);
            display.print("S");
            display.setCursor(0, 3);
            display.print((currentTempSensor + 1));
            
            previousTempSensorChange = currentMillis;
        }
    }

    // Display
    // ==============================================================================
    if ((currentMillis - previousRotate > 1000) && !rotaryButtonClicked) {
        if (currentMillis - previousFanSpeedChange <= 3000) {
            if (currentMillis - previousFanSpeedRefresh >= 500) {
                int rpm = readFanSpeed();
                Serial.println("REFRESH RPM");
                display.clear();
                display.print(rpm);
                //display.showNumberDec(rpm, false, 4, 0);
                previousFanSpeedRefresh = currentMillis;
            }
        }

        // Display current temperature sensor
        if ((currentMillis - previousTempSensorChange >= 1500) && 
                (currentMillis - previousFanSpeedRefresh >= 3000)) {

            int temperature = temperatures[currentTempSensor];

            Serial.print("REFRESH tmp. sensor #");
            Serial.print(currentTempSensor);
            Serial.print(" = ");
            Serial.println(temperature);

            display_degrees(temperature, (currentTempSensor + 1));

            previousTempSensorChange = currentMillis;
        }
    }

    // Refresh all temperatures
    if (currentMillis - previousTempRefresh >= 10000) {
        Serial.println("REFRESH all tmp. sensors");
        
        updateTemperatures();

        highestTempSensor = 0;
        highestTempValue = 0;

        Serial.print("TEMPERATURES: ");

        for (int sensor = 0; sensor < numTempSensors; sensor++) {
            Serial.print("#");
            Serial.print(sensor);
            Serial.print(" = ");
            Serial.print(temperatures[sensor]);    
            Serial.print(" | ");

            if (temperatures[sensor] > highestTempValue) {
                highestTempValue = temperatures[sensor];
                highestTempSensor = sensor;
            }
        }

        Serial.println();
        
        Serial.print("HIGHEST tmp., sensor #");
        Serial.print(highestTempSensor);
        Serial.print(" = ");
        Serial.println(highestTempValue);

        currentTempSensor = highestTempSensor;

        /*
        if ((currentTempValue0 > TEMP_ALERT_LEVEL) || (currentTempValue1 > TEMP_ALERT_LEVEL)
                || (currentTempValue2 > TEMP_ALERT_LEVEL) || (currentTempValue3 > TEMP_ALERT_LEVEL)) {
            highTemperature = true;

            // Activate buzzer
            Serial.println("TEMPERATURE ALARM!");
            NewTone(PIN_PIEZO_BUZZER, 1000, 100);

            if (currentTempValue0 > TEMP_ALERT_LEVEL) currentTempSensor = 0;
            if (currentTempValue1 > TEMP_ALERT_LEVEL) currentTempSensor = 1;
            if (currentTempValue2 > TEMP_ALERT_LEVEL) currentTempSensor = 2;
            if (currentTempValue3 > TEMP_ALERT_LEVEL) currentTempSensor = 3;
        }*/

        previousTempRefresh = currentMillis;
    }
}

// Display degrees
void display_degrees(int temperature, int sensor) {
    int col = 0;
    display.setCursor(0, 0);
    if (temperature < 10) {
        display.print(" ");
        col = 1;
    }
    display.setCursor(0, col);
    display.print(temperature);
    byte rawData;
    rawData = B11100011;
    display.printRaw(rawData, 2);
    display.setCursor(0, 3);

    if (sensor > -1) {
        display.print(sensor);
    } else {
        display.print("C");
    }
}
void display_degrees(int temperature) {
    display_degrees(temperature, -1);
}


// Update temperatures
void updateTemperatures() {
    temperatures[0] = readTemperature(PIN_TMP_0);
    temperatures[1] = readTemperature(PIN_TMP_1);
    temperatures[2] = readTemperature(PIN_TMP_2);
    temperatures[3] = readTemperature(PIN_TMP_3);
}

// Read temperature
float readTemperature(int pin) {
    int raw_adc = analogRead(pin);
    long resistance = 10000*((1024.0 / raw_adc) - 1); 
    float temp = log(resistance); // Saving the Log(resistance) so not to calculate  it 4 times later
    temp = 1 / (0.001129148 + (0.000234125 * temp) + (0.0000000876741 * temp * temp * temp));
    temp = temp - 273.15;  // Convert Kelvin to Celsius                      
    if (isnan(temp) || temp < 0) {
        temp = 0;
    }
    return temp;
}


// Set fan speed
void setFanSpeed(int prc) {
    Serial.print("Setting fan speed at ");
    Serial.print(prc);
    Serial.print(" %, pwm ");
    if (prc >= 0 && prc <= 100) {
        int pwm = ((prc * 255) / 100);
        analogWrite(PIN_FAN_PWM, pwm);
        Serial.print("pwm ");
        Serial.println(pwm);
    }
}

// Read fan speed
int readFanSpeed() {
    long pulseDuration = pulseIn(PIN_FAN_RPM, LOW);
    double frequency = 1000000 / pulseDuration;
    int rpm = 0;
    if (pulseDuration > 0) {
        rpm = (frequency / 2 * 60 / 2);
    }
    return rpm;
}
