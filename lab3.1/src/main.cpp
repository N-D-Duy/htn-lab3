#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include "Controller.h"

#define LED_PIN 32
#define BUTTON_PIN 25
#define DHT_PIN 12
#define HUMIDITY_THRESHOLD 30.0
#define TEMPERATURE_THRESHOLD 35.0
#define DEBOUNCE_DELAY 300
#define OVERRIDE_TIMEOUT 20000

LiquidCrystal_I2C LCD(0x27, 20, 4);
DHT dht(DHT_PIN, DHT22);
Controller *controller;

float humidity, temperature;
volatile bool pumpState = false;
String mode;
unsigned long lastButtonPress = 0;

volatile bool userManualOverride = false;
unsigned long manualOverrideTimestamp = 0;

volatile bool buttonPressed = false;

void IRAM_ATTR handleButtonPress()
{
    buttonPressed = true;
}

void setup()
{
    Serial.begin(115200);

    LCD.init();
    LCD.backlight();

    dht.begin();

    controller = new Controller();
    controller->setup();

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonPress, FALLING);
}

void lcdPrint(float humidity, float temperature, String mode, bool pumpState)
{
    LCD.setCursor(0, 0);
    LCD.print("Humid: ");
    LCD.print(humidity, 1);
    LCD.print("% ");

    LCD.setCursor(0, 1);
    LCD.print("Temp: ");
    LCD.print(temperature, 1);
    LCD.print(" C ");

    LCD.setCursor(0, 2);
    LCD.print("Mode: ");
    LCD.print(mode + "  ");

    LCD.setCursor(0, 3);
    LCD.print("Pump: ");
    LCD.print(pumpState ? " ON " : "OFF");
}

void loop()
{
    controller->streamData();
    String firebaseMode = controller->getMode();
    bool firebasePumpState = controller->isPumpOn();
    if (!controller->isPowerOn())
    {
        digitalWrite(LED_PIN, LOW);
        humidity = dht.readHumidity();
        temperature = dht.readTemperature();

        if (isnan(humidity) || isnan(temperature))
        {
            Serial.println("Failed to read from DHT sensor!");
            return;
        }
        lcdPrint(humidity, temperature, firebaseMode, firebasePumpState);
        controller->setHumidity(humidity);
        controller->setTemperature(temperature);
        delay(500);
        return;
    }

    if (buttonPressed)
    {
        buttonPressed = false;

        unsigned long currentMillis = millis();
        if (currentMillis - lastButtonPress > DEBOUNCE_DELAY)
        {
            lastButtonPress = currentMillis;

            if (firebasePumpState)
            {
                controller->turnPumpOff();
                controller->setMode("AUTO");
                firebaseMode = "AUTO";
                firebasePumpState = false;
            }
            else
            {
                controller->turnPumpOn();
                controller->setMode("MANUAL");
                firebaseMode = "MANUAL";
                firebasePumpState = true;
            }
        }
    }

    if (firebasePumpState)
    {
        digitalWrite(LED_PIN, HIGH);
    }
    else
    {
        digitalWrite(LED_PIN, LOW);
    }

    humidity = dht.readHumidity();
    temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature))
    {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    if (firebaseMode == "AUTO")
    {
        if (humidity < HUMIDITY_THRESHOLD || temperature > TEMPERATURE_THRESHOLD)
        {
            if (!firebasePumpState)
            {
                controller->turnPumpOn();
            }
        }
        else
        {
            if (firebasePumpState)
            {
                controller->turnPumpOff();
            }
        }
    }
    lcdPrint(humidity, temperature, firebaseMode, firebasePumpState);
    delay(500);
}