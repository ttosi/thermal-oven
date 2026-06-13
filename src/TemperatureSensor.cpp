#include "TemperatureSensor.h"

#include "Config.h"

TemperatureSensor::TemperatureSensor(uint8_t sckPin, uint8_t csPin, uint8_t soPin)
    : thermocouple_(sckPin, csPin, soPin) {}

void TemperatureSensor::begin(uint32_t now) {
  startedAtMs_ = now;
  lastSampleAtMs_ = now;
}

void TemperatureSensor::update(uint32_t now) {
  if (now - startedAtMs_ < Config::SENSOR_STARTUP_DELAY_MS ||
      now - lastSampleAtMs_ < Config::TEMPERATURE_SAMPLE_MS) {
    return;
  }

  lastSampleAtMs_ = now;
  attemptedRead_ = true;

  const float reading = thermocouple_.readCelsius();
  if (!isnan(reading) && reading >= -20.0F && reading <= 1024.0F) {
    temperatureC_ = reading;
    lastValidAtMs_ = now;
  } else {
    temperatureC_ = NAN;
  }
}

bool TemperatureSensor::isValid() const {
  return !isnan(temperatureC_);
}

bool TemperatureSensor::isStale(uint32_t now) const {
  if (!attemptedRead_) {
    return false;
  }
  return lastValidAtMs_ == 0 || now - lastValidAtMs_ > Config::SENSOR_STALE_TIMEOUT_MS;
}

float TemperatureSensor::temperatureC() const {
  return temperatureC_;
}
