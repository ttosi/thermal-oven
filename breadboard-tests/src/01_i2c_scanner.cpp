#include <Arduino.h>
#include <Wire.h>

namespace {

constexpr uint32_t SCAN_INTERVAL_MS = 3000;
uint32_t lastScanAtMs = 0;

void scanBus() {
  uint8_t devices = 0;
  Serial.println(F("Scanning I2C bus..."));

  for (uint8_t address = 1; address < 127; ++address) {
    Wire.beginTransmission(address);
    const uint8_t result = Wire.endTransmission();
    if (result == 0) {
      Serial.print(F("Found device at 0x"));
      if (address < 16) {
        Serial.print('0');
      }
      Serial.println(address, HEX);
      ++devices;
    }
  }

  if (devices == 0) {
    Serial.println(F("No I2C devices found."));
  }
  Serial.println();
}

}  // namespace

void setup() {
  Serial.begin(115200);
  Wire.begin();
  scanBus();
  lastScanAtMs = millis();
}

void loop() {
  const uint32_t now = millis();
  if (now - lastScanAtMs >= SCAN_INTERVAL_MS) {
    lastScanAtMs = now;
    scanBus();
  }
}
