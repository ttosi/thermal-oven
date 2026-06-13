#include "UserInterface.h"

#include "Config.h"

namespace {

constexpr int8_t ENCODER_TRANSITIONS[16] = {
    0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0,
};

}  // namespace

UserInterface::UserInterface(uint8_t encoderClkPin, uint8_t encoderDtPin,
                             uint8_t switchPin)
    : lcd_(Config::LCD_ADDRESS, Config::LCD_COLUMNS, Config::LCD_ROWS),
      encoderClkPin_(encoderClkPin),
      encoderDtPin_(encoderDtPin),
      switchPin_(switchPin) {}

void UserInterface::begin(uint32_t now) {
  pinMode(encoderClkPin_, INPUT_PULLUP);
  pinMode(encoderDtPin_, INPUT_PULLUP);
  pinMode(switchPin_, INPUT_PULLUP);

  previousEncoderState_ =
      (digitalRead(encoderClkPin_) << 1) | digitalRead(encoderDtPin_);
  rawButtonState_ = digitalRead(switchPin_);
  stableButtonState_ = rawButtonState_;
  rawButtonChangedAtMs_ = now;

  lcd_.init();
  lcd_.backlight();
  lcd_.clear();
  lcd_.setCursor(0, 0);
  lcd_.print(F("Thermal Oven"));
  lcd_.setCursor(0, 1);
  lcd_.print(F("Initializing..."));
}

void UserInterface::update(uint32_t now) {
  const uint8_t encoderState =
      (digitalRead(encoderClkPin_) << 1) | digitalRead(encoderDtPin_);
  if (encoderState != previousEncoderState_) {
    const uint8_t transition = (previousEncoderState_ << 2) | encoderState;
    encoderAccumulator_ += ENCODER_TRANSITIONS[transition];
    previousEncoderState_ = encoderState;

    if (encoderAccumulator_ >= 4) {
      ++pendingSteps_;
      encoderAccumulator_ = 0;
    } else if (encoderAccumulator_ <= -4) {
      --pendingSteps_;
      encoderAccumulator_ = 0;
    }
  }

  const bool rawButton = digitalRead(switchPin_);
  if (rawButton != rawButtonState_) {
    rawButtonState_ = rawButton;
    rawButtonChangedAtMs_ = now;
  }

  if (rawButtonState_ != stableButtonState_ &&
      now - rawButtonChangedAtMs_ >= Config::BUTTON_DEBOUNCE_MS) {
    stableButtonState_ = rawButtonState_;
    if (stableButtonState_ == LOW) {
      buttonPressed_ = true;
    }
  }
}

int8_t UserInterface::consumeEncoderSteps() {
  const int8_t steps = pendingSteps_;
  pendingSteps_ = 0;
  return steps;
}

bool UserInterface::consumeButtonPress() {
  const bool pressed = buttonPressed_;
  buttonPressed_ = false;
  return pressed;
}

void UserInterface::render(float actualC, float targetC, UiStatus status,
                           bool heaterOn, uint32_t now) {
  if (now - lastDisplayAtMs_ < Config::DISPLAY_REFRESH_MS) {
    return;
  }
  lastDisplayAtMs_ = now;

  lcd_.setCursor(0, 0);
  lcd_.print(F("Now:"));
  printTemperature(actualC);
  lcd_.print(F("C     "));

  lcd_.setCursor(0, 1);
  if (status == UiStatus::SensorFault) {
    lcd_.print(F("FAULT: SENSOR   "));
    return;
  }
  if (status == UiStatus::OverTemperature) {
    lcd_.print(F("FAULT: OVER TEMP"));
    return;
  }

  lcd_.print(F("Set:"));
  printTemperature(targetC);
  lcd_.print(F("C "));
  if (status == UiStatus::Running) {
    lcd_.print(heaterOn ? F("HEAT") : F("RUN "));
  } else {
    lcd_.print(F("OFF "));
  }
}

void UserInterface::printTemperature(float temperatureC) {
  if (isnan(temperatureC)) {
    lcd_.print(F("---"));
    return;
  }

  const int rounded = static_cast<int>(temperatureC + 0.5F);
  if (rounded < 100) {
    lcd_.print(' ');
  }
  if (rounded < 10) {
    lcd_.print(' ');
  }
  lcd_.print(rounded);
}
