#include <Wire.h>
#include <Arduino.h>
#include "Controller.h"
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>


#define HARDWARE_TYPE MD_MAX72XX::PAROLA_HW
#define MAX_DEVICES 4
#define CS_PIN 21
#define CLK_PIN 13
#define DATA_PIN 11

#define BUF_SIZE 75
uint8_t scrollSpeed = 75;
textEffect_t scrollEffect = PA_SCROLL_LEFT;
textPosition_t scrollAlign = PA_LEFT;
uint16_t scrollPause = 0;

char curMessage[BUF_SIZE] = { "" };
char newMessage[BUF_SIZE] = { "" };
bool newMessageAvailable = false;

MD_Parola ledMatrix = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
Controller* controller;
TaskHandle_t ledTaskHandle;

void displayTextTask(void* parameter);

void setup()
{
    Serial.begin(115200);

    controller = new Controller();
    controller->setup();

    ledMatrix.begin();
    ledMatrix.setIntensity(5);
    ledMatrix.displayClear();

    ledMatrix.displayText(
        curMessage, 
        scrollAlign, 
        scrollSpeed, 
        scrollPause, 
        scrollEffect, 
        scrollEffect
    );

    xTaskCreatePinnedToCore(
        displayTextTask,
        "LED Display Task",
        4096,
        NULL,
        1,
        &ledTaskHandle,
        1
    );
}

void loop()
{
    controller->streamData();
    delay(10);
}

void displayTextTask(void* parameter)
{
    while (true)
    {
        String text = controller->getText();
        if (!text.isEmpty())
        {
            text.toCharArray(newMessage, BUF_SIZE);
            newMessageAvailable = true;
        }
        if (ledMatrix.displayAnimate())
        {
            if (newMessageAvailable)
            {
                strcpy(curMessage, newMessage);
                newMessageAvailable = false;
            }

            ledMatrix.displayReset();
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}