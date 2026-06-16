#include <Arduino.h>
#include <max6675.h>

#include "../include/TestPins.h"

namespace
{

constexpr uint32_t READ_INTERVAL_MS = 500;
MAX6675 thermocouple(TestPins::THERMOCOUPLE_SCK, TestPins::THERMOCOUPLE_CS, TestPins::THERMOCOUPLE_SO);
uint32_t lastReadAtMs = 0;

} // namespace

void setup()
{
    Serial.begin(115200);
    Serial.println(F("MAX6675 test; waiting for first conversion..."));
}

void loop()
{
    const uint32_t now = millis();
    if (now < 750 || now - lastReadAtMs < READ_INTERVAL_MS)
    {
        return;
    }
    lastReadAtMs = now;

    const float temperatureC = thermocouple.readCelsius();
    if (isnan(temperatureC))
    {
        Serial.println(F("FAULT: invalid reading; check probe and wiring."));
    }
    else
    {
        Serial.print(F("Temperature: "));
        Serial.print(temperatureC, 2);
        Serial.println(F(" C"));
    }
}
