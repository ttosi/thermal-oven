#include <Arduino.h>
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

#include "../include/TestPins.h"

namespace
{

  hd44780_I2Cexp lcd;
  uint32_t lastUpdateAtMs = 0;
  uint32_t counter = 0;

} // namespace

void setup()
{
  Serial.begin(115200);
  Serial.println(F("LCD auto-detection test"));

  const int status = lcd.begin(TestPins::LCD_COLUMNS, TestPins::LCD_ROWS);
  if (status != 0)
  {
    Serial.print(F("LCD initialization failed, status="));
    Serial.println(status);
    Serial.println(F("Run i2c_scanner and check 5V, GND, SDA, SCL, and soldering."));
    while (true)
    {
    }
  }

  Serial.println(F("LCD detected and initialized."));
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(F("LCD1602 test"));
}

void loop()
{
  const uint32_t now = millis();
  if (now - lastUpdateAtMs < 500)
  {
    return;
  }
  lastUpdateAtMs = now;

  lcd.setCursor(0, 1);
  lcd.print(F("Count: "));
  lcd.print(counter++);
  lcd.print(F("       "));
}
