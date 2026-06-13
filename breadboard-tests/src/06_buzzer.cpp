#include <Arduino.h>

#include "../include/TestPins.h"

namespace {

constexpr uint32_t BEEP_INTERVAL_MS = 2000;
constexpr uint32_t BEEP_DURATION_MS = 150;

}  // namespace

void setup() {
  pinMode(TestPins::BUZZER, OUTPUT);
  digitalWrite(TestPins::BUZZER, LOW);
}

void loop() {
  const uint32_t phase = millis() % BEEP_INTERVAL_MS;
  digitalWrite(TestPins::BUZZER, phase < BEEP_DURATION_MS ? HIGH : LOW);
}
