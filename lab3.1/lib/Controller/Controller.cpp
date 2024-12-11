#include "Controller.h"

Controller::Controller() : service()
{
}

void Controller::setup()
{
    service.initService();
}

void Controller::turnPumpOn()
{
    service.turnPumpOn();
}

void Controller::turnPumpOff()
{
    service.turnPumpOff();
}

boolean Controller::isPumpOn()
{
    return service.isPumpOn;
}

float Controller::getHumidity()
{
    return service.humidity;
}

float Controller::getTemperature()
{
    return service.temperature;
}

void Controller::streamData()
{
    service.getFirebaseData();
}

void Controller::setHumidity(float humidity)
{
    service.setHumidity(humidity);
}

void Controller::setTemperature(float temperature)
{
    service.setTemperature(temperature);
}