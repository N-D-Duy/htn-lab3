#ifndef SERVICE_H
#define SERVICE_H

#include <Firebase_ESP_Client.h>

class Service {
public:
    Service();
    void getFirebaseData();
    void initService();
    void turnPumpOn();
    void turnPumpOff();
    void setHumidity(float humidity);
    void setTemperature(float temperature);
    void setMode(String mode);
    float humidity;
    float temperature;
    String mode;
    boolean isPumpOn;
    boolean isPowerOn;
    void turnPowerOn();
    void turnPowerOff();
};

#endif