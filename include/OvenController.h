#pragma once

#include <Arduino.h>

#include "HeaterController.h"
#include "TemperatureSensor.h"
#include "UserInterface.h"

class OvenController
{
  public:
    OvenController();

    void begin();
    void update();

  private:
    enum class State : uint8_t
    {
        Off,
        Running,
        SensorFault,
        OverTemperature,
    };

    void handleInput(uint32_t now);
    void evaluateSafety(uint32_t now);
    void stop(State nextState);
    UiStatus uiStatus() const;
    void logStatus(uint32_t now);

    TemperatureSensor temperatureSensor_;
    HeaterController heater_;
    UserInterface ui_;
    State state_ = State::Off;
    float targetC_;
    uint32_t lastLogAtMs_ = 0;
};
