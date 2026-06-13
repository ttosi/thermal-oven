# Breadboard Component Tests

Temporary, isolated firmware programs for testing each low-voltage component
before assembling the complete controller.

Each source file is its own PlatformIO environment. No source files need to be
renamed or edited when switching tests.

Open `breadboard-tests` as the PlatformIO project directory when using an IDE.
The test sources also use an explicit relative path to `include/TestPins.h`, so
the shared pin definitions remain discoverable when inspecting files
individually.

## Safety

The SSR tests are for verifying the Arduino-side transistor driver with an LED,
multimeter, or disconnected SSR input only.

**Do not connect mains voltage or the oven heating elements while running these
tests.** Independent mains safety hardware must be installed and reviewed
before any powered heater test.

## Test Environments

| Environment | Source | Purpose |
|---|---|---|
| `i2c_scanner` | `src/01_i2c_scanner.cpp` | Finds the LCD adapter I2C address |
| `lcd1602` | `src/02_lcd1602.cpp` | Displays a continuously increasing counter |
| `rotary_encoder` | `src/03_rotary_encoder.cpp` | Reports rotation and button events over serial |
| `max6675` | `src/04_max6675.cpp` | Reports thermocouple temperature over serial |
| `ssr_driver` | `src/05_ssr_driver.cpp` | Cycles the low-voltage driver output, then stops |
| `buzzer` | `src/06_buzzer.cpp` | Beeps an active buzzer connected to `D9` |
| `low_voltage_integration` | `src/07_low_voltage_integration.cpp` | Tests LCD, encoder, MAX6675, and SSR driver together |

## Build and Upload

Run commands from this directory:

```bash
cd breadboard-tests
```

Build one test:

```bash
/home/ttosi/.platformio/penv/bin/pio run -e lcd1602
```

Upload one test:

```bash
/home/ttosi/.platformio/penv/bin/pio run -e lcd1602 --target upload
```

Open the serial monitor for tests that produce serial output:

```bash
/home/ttosi/.platformio/penv/bin/pio device monitor --baud 115200
```

## Recommended Test Order

1. `i2c_scanner`
2. `lcd1602`
3. `rotary_encoder`
4. `max6675`
5. `buzzer`
6. `ssr_driver` using an LED or meter only
7. `low_voltage_integration` using an LED or meter only

## Pin Map

| Function | Nano Pin |
|---|---|
| Encoder `CLK` | `D2` |
| Encoder `DT` | `D3` |
| Encoder button `SW` | `D4` |
| SSR driver | `D5` |
| MAX6675 `CS` | `D6` |
| MAX6675 `SCK` | `D7` |
| MAX6675 `SO` | `D8` |
| Active buzzer | `D9` |
| LCD I2C `SDA` | `A4` |
| LCD I2C `SCL` | `A5` |

All low-voltage modules must share controller `GND`.
