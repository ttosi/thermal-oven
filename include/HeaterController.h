#pragma once

#include <Arduino.h>
#include <PID_v1.h>

class HeaterController {
 public:
  explicit HeaterController(uint8_t ssrPin);

  void begin(uint32_t now);
  void start(double targetC, double actualC, uint32_t now);
  void stop();
  void setTarget(double targetC);
  void update(double actualC, uint32_t now);

  bool isRunning() const;
  bool isHeaterOn() const;
  double outputPercent() const;

 private:
  void setHeater(bool on);

  uint8_t ssrPin_;
  double inputC_ = 0.0;
  double targetC_ = 0.0;
  double outputMs_ = 0.0;
  PID pid_;
  uint32_t windowStartedAtMs_ = 0;
  bool running_ = false;
  bool heaterOn_ = false;
};
