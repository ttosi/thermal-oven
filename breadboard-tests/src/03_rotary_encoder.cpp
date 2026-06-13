#include <Arduino.h>

#include "../include/TestPins.h"

namespace {

constexpr int8_t TRANSITIONS[16] = {
    0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0,
};
constexpr uint32_t DEBOUNCE_MS = 40;

uint8_t previousEncoderState = 0;
int8_t encoderAccumulator = 0;
long position = 0;
bool rawButton = HIGH;
bool stableButton = HIGH;
uint32_t rawButtonChangedAtMs = 0;

}  // namespace

void setup() {
  Serial.begin(115200);
  pinMode(TestPins::ENCODER_CLK, INPUT_PULLUP);
  pinMode(TestPins::ENCODER_DT, INPUT_PULLUP);
  pinMode(TestPins::ENCODER_SWITCH, INPUT_PULLUP);

  previousEncoderState =
      (digitalRead(TestPins::ENCODER_CLK) << 1) |
      digitalRead(TestPins::ENCODER_DT);
  rawButton = digitalRead(TestPins::ENCODER_SWITCH);
  stableButton = rawButton;

  Serial.println(F("Rotate encoder and press its button."));
}

void loop() {
  const uint32_t now = millis();
  const uint8_t encoderState =
      (digitalRead(TestPins::ENCODER_CLK) << 1) |
      digitalRead(TestPins::ENCODER_DT);

  if (encoderState != previousEncoderState) {
    encoderAccumulator += TRANSITIONS[(previousEncoderState << 2) | encoderState];
    previousEncoderState = encoderState;

    if (encoderAccumulator >= 4 || encoderAccumulator <= -4) {
      position += encoderAccumulator > 0 ? 1 : -1;
      encoderAccumulator = 0;
      Serial.print(F("Position: "));
      Serial.println(position);
    }
  }

  const bool currentRawButton = digitalRead(TestPins::ENCODER_SWITCH);
  if (currentRawButton != rawButton) {
    rawButton = currentRawButton;
    rawButtonChangedAtMs = now;
  }

  if (rawButton != stableButton && now - rawButtonChangedAtMs >= DEBOUNCE_MS) {
    stableButton = rawButton;
    Serial.println(stableButton == LOW ? F("Button pressed")
                                      : F("Button released"));
  }
}
