#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Service.h"

class Controller
{
public:
    Controller();
    void setup();
    void turnPumpOn();
    void turnPumpOff();
    boolean isPumpOn();
    float getHumidity();
    void setHumidity(float humidity);
    float getTemperature();
    void setTemperature(float temperature);
    String getMode();
    void setMode(String mode);
    boolean isPowerOn();
    void turnPowerOn();
    void turnPowerOff();
    void streamData();
private:
    Service service; 
};

#endif
