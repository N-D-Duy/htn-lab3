#ifndef SERVICE_H
#define SERVICE_H

#include <Firebase_ESP_Client.h>

class Service {
public:
    Service();
    void getFirebaseData();
    void initService();
    void changeText(String text);
    String text;
};

#endif