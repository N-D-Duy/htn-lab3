#include "Controller.h"

Controller::Controller() : service()
{
}

void Controller::setup()
{
    service.initService();
}

void Controller::changeText(String text)
{
    service.changeText(text);
}

String Controller::getText()
{
    return service.text;
}

void Controller::streamData()
{
    service.getFirebaseData();
}