#include <Arduino.h>
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include <max6675.h>

#include "../include/TestPins.h"

namespace
{

constexpr uint32_t READ_INTERVAL_MS = 500;
constexpr uint32_t DISPLAY_INTERVAL_MS = 250;
constexpr uint32_t DEBOUNCE_MS = 40;
constexpr int8_t TRANSITIONS[16] = {
    0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0,
};

hd44780_I2Cexp lcd;
MAX6675 thermocouple(TestPins::THERMOCOUPLE_SCK, TestPins::THERMOCOUPLE_CS, TestPins::THERMOCOUPLE_SO);

uint8_t previousEncoderState = 0;
int8_t encoderAccumulator = 0;
int targetC = 100;
float actualC = NAN;
bool rawButton = HIGH;
bool stableButton = HIGH;
bool outputEnabled = false;
uint32_t rawButtonChangedAtMs = 0;
uint32_t lastReadAtMs = 0;
uint32_t lastDisplayAtMs = 0;

void setOutput(bool on)
{
    outputEnabled = on;
    digitalWrite(TestPins::SSR_DRIVER, on ? HIGH : LOW);
}

} // namespace

void setup()
{
    Serial.begin(115200);
    pinMode(TestPins::ENCODER_CLK, INPUT_PULLUP);
    pinMode(TestPins::ENCODER_DT, INPUT_PULLUP);
    pinMode(TestPins::ENCODER_SWITCH, INPUT_PULLUP);

    digitalWrite(TestPins::SSR_DRIVER, LOW);
    pinMode(TestPins::SSR_DRIVER, OUTPUT);
    digitalWrite(TestPins::SSR_DRIVER, LOW);

    previousEncoderState = (digitalRead(TestPins::ENCODER_CLK) << 1) | digitalRead(TestPins::ENCODER_DT);
    rawButton = digitalRead(TestPins::ENCODER_SWITCH);
    stableButton = rawButton;

    const int lcdStatus = lcd.begin(TestPins::LCD_COLUMNS, TestPins::LCD_ROWS);
    if (lcdStatus != 0)
    {
        Serial.print(F("LCD initialization failed, status="));
        Serial.println(lcdStatus);
        while (true)
        {
        }
    }
    lcd.backlight();

    Serial.println(F("LOW-VOLTAGE TEST ONLY: SSR output must drive an LED or meter."));
}

void loop()
{
    const uint32_t now = millis();
    const uint8_t encoderState = (digitalRead(TestPins::ENCODER_CLK) << 1) | digitalRead(TestPins::ENCODER_DT);

    if (encoderState != previousEncoderState)
    {
        encoderAccumulator += TRANSITIONS[(previousEncoderState << 2) | encoderState];
        previousEncoderState = encoderState;
        if (encoderAccumulator >= 4 || encoderAccumulator <= -4)
        {
            targetC += encoderAccumulator > 0 ? 5 : -5;
            targetC = constrain(targetC, 60, 260);
            encoderAccumulator = 0;
        }
    }

    const bool currentRawButton = digitalRead(TestPins::ENCODER_SWITCH);
    if (currentRawButton != rawButton)
    {
        rawButton = currentRawButton;
        rawButtonChangedAtMs = now;
    }
    if (rawButton != stableButton && now - rawButtonChangedAtMs >= DEBOUNCE_MS)
    {
        stableButton = rawButton;
        if (stableButton == LOW)
        {
            setOutput(!outputEnabled);
        }
    }

    if (now >= 750 && now - lastReadAtMs >= READ_INTERVAL_MS)
    {
        lastReadAtMs = now;
        actualC = thermocouple.readCelsius();
    }

    if (now - lastDisplayAtMs >= DISPLAY_INTERVAL_MS)
    {
        lastDisplayAtMs = now;
        lcd.setCursor(0, 0);
        lcd.print(F("Now:"));
        if (isnan(actualC))
        {
            lcd.print(F("---"));
            setOutput(false);
        }
        else
        {
            lcd.print(static_cast<int>(actualC + 0.5F));
        }
        lcd.print(F("C       "));

        lcd.setCursor(0, 1);
        lcd.print(F("Set:"));
        lcd.print(targetC);
        lcd.print(outputEnabled ? F(" TEST ON ") : F(" TEST OFF"));
    }
}
