#include <Arduino.h>

#include "OvenController.h"

OvenController oven;

void setup()
{
    oven.begin();
}

void loop()
{
    oven.update();
}
