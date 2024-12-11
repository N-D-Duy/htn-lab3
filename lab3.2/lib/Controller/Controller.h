#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Service.h"

class Controller
{
public:
    Controller();
    void setup();
    void changeText(String text);
    String getText();
    void streamData();
private:
    Service service; 
};

#endif
