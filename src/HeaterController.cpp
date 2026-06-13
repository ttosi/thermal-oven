#include "HeaterController.h"

#include "Config.h"

HeaterController::HeaterController(uint8_t ssrPin)
    : ssrPin_(ssrPin),
      pid_(&inputC_, &outputMs_, &targetC_, Config::PID_KP, Config::PID_KI,
           Config::PID_KD, DIRECT) {}

void HeaterController::begin(uint32_t now) {
  // Force a safe output before any other controller initialization.
  digitalWrite(ssrPin_, LOW);
  pinMode(ssrPin_, OUTPUT);
  digitalWrite(ssrPin_, LOW);

  windowStartedAtMs_ = now;
  pid_.SetOutputLimits(0, Config::SSR_WINDOW_MS);
  pid_.SetSampleTime(Config::TEMPERATURE_SAMPLE_MS);
  pid_.SetMode(MANUAL);
}

void HeaterController::start(double targetC, double actualC, uint32_t now) {
  targetC_ = targetC;
  inputC_ = actualC;
  outputMs_ = 0.0;
  windowStartedAtMs_ = now;
  running_ = true;
  pid_.SetMode(AUTOMATIC);
}

void HeaterController::stop() {
  running_ = false;
  pid_.SetMode(MANUAL);
  outputMs_ = 0.0;
  setHeater(false);
}

void HeaterController::setTarget(double targetC) {
  targetC_ = targetC;
}

void HeaterController::update(double actualC, uint32_t now) {
  if (!running_) {
    setHeater(false);
    return;
  }

  inputC_ = actualC;
  pid_.Compute();

  while (now - windowStartedAtMs_ >= Config::SSR_WINDOW_MS) {
    windowStartedAtMs_ += Config::SSR_WINDOW_MS;
  }

  setHeater((now - windowStartedAtMs_) < static_cast<uint32_t>(outputMs_));
}

bool HeaterController::isRunning() const {
  return running_;
}

bool HeaterController::isHeaterOn() const {
  return heaterOn_;
}

double HeaterController::outputPercent() const {
  return outputMs_ * 100.0 / Config::SSR_WINDOW_MS;
}

void HeaterController::setHeater(bool on) {
  heaterOn_ = on;
  digitalWrite(ssrPin_, on ? HIGH : LOW);
}
