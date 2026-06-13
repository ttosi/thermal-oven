#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#include "../include/TestPins.h"

namespace {

LiquidCrystal_I2C lcd(TestPins::LCD_ADDRESS, TestPins::LCD_COLUMNS,
                      TestPins::LCD_ROWS);
uint32_t lastUpdateAtMs = 0;
uint32_t counter = 0;

}  // namespace

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("LCD1602 test"));
}

void loop() {
  const uint32_t now = millis();
  if (now - lastUpdateAtMs < 500) {
    return;
  }
  lastUpdateAtMs = now;

  lcd.setCursor(0, 1);
  lcd.print(F("Count: "));
  lcd.print(counter++);
  lcd.print(F("       "));
}
