#pragma once

#include <Arduino.h>
#include <max6675.h>

class TemperatureSensor {
 public:
  TemperatureSensor(uint8_t sckPin, uint8_t csPin, uint8_t soPin);

  void begin(uint32_t now);
  void update(uint32_t now);

  bool isValid() const;
  bool isStale(uint32_t now) const;
  float temperatureC() const;

 private:
  MAX6675 thermocouple_;
  float temperatureC_ = NAN;
  uint32_t startedAtMs_ = 0;
  uint32_t lastSampleAtMs_ = 0;
  uint32_t lastValidAtMs_ = 0;
  bool attemptedRead_ = false;
};
