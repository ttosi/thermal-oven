#pragma once

#include <Arduino.h>

namespace Config {

constexpr uint8_t PIN_ENCODER_CLK = 2;
constexpr uint8_t PIN_ENCODER_DT = 3;
constexpr uint8_t PIN_ENCODER_SWITCH = 4;
constexpr uint8_t PIN_SSR = 5;
constexpr uint8_t PIN_THERMOCOUPLE_CS = 6;
constexpr uint8_t PIN_THERMOCOUPLE_SCK = 7;
constexpr uint8_t PIN_THERMOCOUPLE_SO = 8;

constexpr uint8_t LCD_ADDRESS = 0x27;
constexpr uint8_t LCD_COLUMNS = 16;
constexpr uint8_t LCD_ROWS = 2;

constexpr float MIN_TARGET_C = 60.0F;
constexpr float MAX_TARGET_C = 260.0F;
constexpr float TARGET_STEP_C = 5.0F;
constexpr float DEFAULT_TARGET_C = 100.0F;
constexpr float MAX_SAFE_C = 280.0F;

constexpr uint32_t TEMPERATURE_SAMPLE_MS = 500;
constexpr uint32_t DISPLAY_REFRESH_MS = 250;
constexpr uint32_t SSR_WINDOW_MS = 2000;
constexpr uint32_t BUTTON_DEBOUNCE_MS = 40;
constexpr uint32_t SENSOR_STARTUP_DELAY_MS = 750;
constexpr uint32_t SENSOR_STALE_TIMEOUT_MS = 2500;

// Conservative starting values. Tune these using logged data from the real oven.
constexpr double PID_KP = 8.0;
constexpr double PID_KI = 0.25;
constexpr double PID_KD = 30.0;

}  // namespace Config
