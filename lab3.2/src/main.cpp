#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include "Controller.h"
#include <string.h>

LiquidCrystal_I2C LCD(0x27, 16, 2);
Controller* controller;
String* currentText = nullptr;
TaskHandle_t lcdTaskHandle;

void displayTextTask(void* parameter);

void setup()
{
    Serial.begin(115200);
    LCD.init();
    LCD.backlight();
    controller = new Controller();
    controller->setup();
    
    xTaskCreatePinnedToCore(
        displayTextTask,
        "LCD Display Task",
        4096,
        NULL,
        1,
        &lcdTaskHandle,
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
        
        if (!text.isEmpty() && text != (currentText ? *currentText : ""))
        {
            LCD.clear();
            
            if (text.length() > 16)
            {
                int textLength = text.length();
                int scrollOffset = 0;
                
                while (true)
                {
                    String displayText;
                    if (scrollOffset + 16 <= textLength)
                    {
                        displayText = text.substring(scrollOffset, scrollOffset + 16);
                    }
                    else
                    {
                        int remainingChars = textLength - scrollOffset;
                        displayText = text.substring(scrollOffset) + 
                                      text.substring(0, 16 - remainingChars);
                    }
                    
                    LCD.setCursor(0, 0);
                    LCD.print(displayText);
                    
                    vTaskDelay(300 / portTICK_PERIOD_MS);
                    
                    
                    scrollOffset = (scrollOffset + 1) % textLength;
                    
                    
                    String currentTextCheck = controller->getText();
                    if (currentTextCheck != text)
                        break;
                }
            }
            else
            {
                LCD.print(text);
            }
            
            // Update current text
            if (currentText) delete currentText;
            currentText = new String(text);
        }
        
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}