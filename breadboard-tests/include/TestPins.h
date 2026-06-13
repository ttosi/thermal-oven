#pragma once

#include <Arduino.h>

namespace TestPins {

constexpr uint8_t ENCODER_CLK = 2;
constexpr uint8_t ENCODER_DT = 3;
constexpr uint8_t ENCODER_SWITCH = 4;
constexpr uint8_t SSR_DRIVER = 5;
constexpr uint8_t THERMOCOUPLE_CS = 6;
constexpr uint8_t THERMOCOUPLE_SCK = 7;
constexpr uint8_t THERMOCOUPLE_SO = 8;
constexpr uint8_t BUZZER = 9;

constexpr uint8_t LCD_ADDRESS = 0x27;
constexpr uint8_t LCD_COLUMNS = 16;
constexpr uint8_t LCD_ROWS = 2;

}  // namespace TestPins
