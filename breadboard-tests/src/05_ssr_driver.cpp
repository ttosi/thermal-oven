#include <Arduino.h>

#include "../include/TestPins.h"

namespace
{

constexpr uint32_t ON_TIME_MS = 1000;
constexpr uint32_t CYCLE_TIME_MS = 3000;
constexpr uint32_t MAX_TEST_TIME_MS = 30000;
bool outputOn = false;

void setOutput(bool on)
{
    if (outputOn == on)
    {
        return;
    }
    outputOn = on;
    digitalWrite(TestPins::SSR_DRIVER, on ? HIGH : LOW);
    Serial.println(on ? F("SSR driver ON") : F("SSR driver OFF"));
}

} // namespace

void setup()
{
    Serial.begin(115200);

    digitalWrite(TestPins::SSR_DRIVER, LOW);
    pinMode(TestPins::SSR_DRIVER, OUTPUT);
    digitalWrite(TestPins::SSR_DRIVER, LOW);

    Serial.println(F("SSR DRIVER TEST: use an LED or meter only; do not connect mains."));
    Serial.println(F("Output cycles 1 s ON / 2 s OFF, then stops after 30 s."));
}

void loop()
{
    const uint32_t now = millis();
    if (now >= MAX_TEST_TIME_MS)
    {
        setOutput(false);
        return;
    }

    setOutput((now % CYCLE_TIME_MS) < ON_TIME_MS);
}
