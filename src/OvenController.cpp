#include "OvenController.h"

#include "Config.h"

OvenController::OvenController()
    : temperatureSensor_(Config::PIN_THERMOCOUPLE_SCK,
                         Config::PIN_THERMOCOUPLE_CS,
                         Config::PIN_THERMOCOUPLE_SO),
      heater_(Config::PIN_SSR),
      ui_(Config::PIN_ENCODER_CLK, Config::PIN_ENCODER_DT,
          Config::PIN_ENCODER_SWITCH),
      targetC_(Config::DEFAULT_TARGET_C) {}

void OvenController::begin() {
  const uint32_t now = millis();

  Serial.begin(115200);
  heater_.begin(now);
  temperatureSensor_.begin(now);
  ui_.begin(now);

  Serial.println(F("ms,state,actual_c,target_c,output_percent,heater"));
}

void OvenController::update() {
  const uint32_t now = millis();

  ui_.update(now);
  temperatureSensor_.update(now);
  handleInput(now);
  evaluateSafety(now);

  if (state_ == State::Running && temperatureSensor_.isValid()) {
    heater_.setTarget(targetC_);
    heater_.update(temperatureSensor_.temperatureC(), now);
  } else {
    heater_.stop();
  }

  ui_.render(temperatureSensor_.temperatureC(), targetC_, uiStatus(),
             heater_.isHeaterOn(), now);
  logStatus(now);
}

void OvenController::handleInput(uint32_t now) {
  const int8_t steps = ui_.consumeEncoderSteps();
  if (state_ == State::Off && steps != 0) {
    targetC_ += steps * Config::TARGET_STEP_C;
    targetC_ = constrain(targetC_, Config::MIN_TARGET_C, Config::MAX_TARGET_C);
  }

  if (!ui_.consumeButtonPress()) {
    return;
  }

  if (state_ == State::Running) {
    stop(State::Off);
    return;
  }

  if (state_ == State::Off && temperatureSensor_.isValid() &&
      !temperatureSensor_.isStale(now)) {
    heater_.start(targetC_, temperatureSensor_.temperatureC(), now);
    state_ = State::Running;
  }
}

void OvenController::evaluateSafety(uint32_t now) {
  if (temperatureSensor_.isStale(now)) {
    stop(State::SensorFault);
    return;
  }

  if (temperatureSensor_.isValid() &&
      temperatureSensor_.temperatureC() >= Config::MAX_SAFE_C) {
    stop(State::OverTemperature);
  }
}

void OvenController::stop(State nextState) {
  heater_.stop();
  state_ = nextState;
}

UiStatus OvenController::uiStatus() const {
  switch (state_) {
    case State::Running:
      return UiStatus::Running;
    case State::SensorFault:
      return UiStatus::SensorFault;
    case State::OverTemperature:
      return UiStatus::OverTemperature;
    case State::Off:
    default:
      return UiStatus::Off;
  }
}

void OvenController::logStatus(uint32_t now) {
  if (now - lastLogAtMs_ < 1000) {
    return;
  }
  lastLogAtMs_ = now;

  Serial.print(now);
  Serial.print(',');
  Serial.print(static_cast<uint8_t>(state_));
  Serial.print(',');
  Serial.print(temperatureSensor_.temperatureC());
  Serial.print(',');
  Serial.print(targetC_);
  Serial.print(',');
  Serial.print(heater_.outputPercent());
  Serial.print(',');
  Serial.println(heater_.isHeaterOn() ? 1 : 0);
}
