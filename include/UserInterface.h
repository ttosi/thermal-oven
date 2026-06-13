#pragma once

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

enum class UiStatus : uint8_t {
  Off,
  Running,
  SensorFault,
  OverTemperature,
};

class UserInterface {
 public:
  UserInterface(uint8_t encoderClkPin, uint8_t encoderDtPin, uint8_t switchPin);

  void begin(uint32_t now);
  void update(uint32_t now);
  int8_t consumeEncoderSteps();
  bool consumeButtonPress();
  void render(float actualC, float targetC, UiStatus status, bool heaterOn,
              uint32_t now);

 private:
  void printTemperature(float temperatureC);

  LiquidCrystal_I2C lcd_;
  uint8_t encoderClkPin_;
  uint8_t encoderDtPin_;
  uint8_t switchPin_;
  uint8_t previousEncoderState_ = 0;
  int8_t encoderAccumulator_ = 0;
  int8_t pendingSteps_ = 0;
  bool rawButtonState_ = HIGH;
  bool stableButtonState_ = HIGH;
  bool buttonPressed_ = false;
  uint32_t rawButtonChangedAtMs_ = 0;
  uint32_t lastDisplayAtMs_ = 0;
};
