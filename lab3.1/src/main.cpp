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

LiquidCrystal_I2C LCD(0x27, 20, 4);
DHT dht(DHT_PIN, DHT22);
Controller *controller;

float humidity, temperature;
volatile bool manualPumpControl = false;
volatile bool pumpState = false;
bool previousManualPumpControl = false;
unsigned long lastButtonPress = 0;

void IRAM_ATTR handleButtonPress()
{
    unsigned long currentMillis = millis();
    if (currentMillis - lastButtonPress > DEBOUNCE_DELAY)
    {
        lastButtonPress = currentMillis;
        manualPumpControl = !manualPumpControl;
        pumpState = !pumpState;
        digitalWrite(LED_PIN, pumpState ? HIGH : LOW);
    }
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

void loop()
{
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();

    if (previousManualPumpControl && !manualPumpControl)
    {
        pumpState = false;
        digitalWrite(LED_PIN, LOW);
        controller->turnPumpOff();
    }

    if (!manualPumpControl)
    {
        if (humidity < HUMIDITY_THRESHOLD || temperature > TEMPERATURE_THRESHOLD)
        {
            if (!pumpState)
            {
                pumpState = true;
                digitalWrite(LED_PIN, HIGH);
                controller->turnPumpOn();
            }
        }
        else
        {
            if (pumpState)
            {
                pumpState = false;
                digitalWrite(LED_PIN, LOW);
                controller->turnPumpOff();
            }
        }
    }
    else
    {
        // Điều khiển thủ công: đồng bộ trạng thái bơm với Firebase
        bool firebasePumpState = controller->isPumpOn();
        if (pumpState != firebasePumpState)
        {
            if (pumpState)
                controller->turnPumpOn();
            else
                controller->turnPumpOff();
        }
    }

    if (humidity != controller->getHumidity())
    {
        controller->setHumidity(humidity);
    }
    if (temperature != controller->getTemperature())
    {
        controller->setTemperature(temperature);
    }

    LCD.setCursor(0, 0);
    LCD.print("Humid: ");
    LCD.print(humidity);
    LCD.print("%");
    LCD.setCursor(0, 1);
    LCD.print("Temp: ");
    LCD.print(temperature);
    LCD.print(" C");
    LCD.setCursor(0, 2);
    LCD.print("Pump: ");
    LCD.print(pumpState ? "-ON" : "OFF");

    previousManualPumpControl = manualPumpControl;

    delay(500);
}
