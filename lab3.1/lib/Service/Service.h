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
    float humidity;
    float temperature;
    boolean isPumpOn;
};

#endif