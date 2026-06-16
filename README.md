# PID Thermal Oven Controller

Arduino Nano firmware and hardware documentation for converting a conventional
toaster oven into a PID-controlled thermal oven.

Intended uses include:

- Controlled heating of resin-printed dice masters
- SMD solder reflow

This is a prototype involving hazardous mains voltage and high temperatures.
The Arduino and SSR must not be the only protection against uncontrolled
heating.

## Features

- PID temperature control using a K-type thermocouple and MAX6675 module
- Adjustable `60-260 C` target in `5 C` increments
- Rotary encoder control with push-to-start and push-to-stop
- LCD1602 display showing actual temperature, target, and heater status
- Non-blocking firmware architecture
- Two-second time-proportioned SSR control window
- Sensor-failure and `280 C` over-temperature shutdown
- Serial CSV output for PID tuning

## Operation

1. Power on the controller.
2. Rotate the encoder while stopped to select the target temperature.
3. Press the encoder button to start heating.
4. Press the button while running to immediately command the heater off.

Sensor or over-temperature faults latch the heater off until the controller is
power-cycled.

## Firmware Pin Map

| Function | Arduino Nano Pin |
|---|---|
| Encoder `CLK` | `D2` |
| Encoder `DT` | `D3` |
| Encoder button `SW` | `D4` |
| SSR transistor driver | `D5` |
| MAX6675 `CS` | `D6` |
| MAX6675 `SCK` | `D7` |
| MAX6675 `SO` | `D8` |
| LCD I2C `SDA` | `A4` |
| LCD I2C `SCL` | `A5` |

See [PIN_MAPPING.md](PIN_MAPPING.md) for complete component connections.

## Firmware Structure

| Module | Responsibility |
|---|---|
| `Config` | Pin assignments, limits, timing, and PID constants |
| `TemperatureSensor` | Non-blocking MAX6675 sampling and validation |
| `HeaterController` | PID calculation and SSR time-proportioned output |
| `UserInterface` | LCD output, encoder decoding, and button debounce |
| `OvenController` | Operating state, input handling, and safety shutdowns |

## Build

The project uses PlatformIO with the Arduino framework.

```bash
/home/ttosi/.platformio/penv/bin/pio run
```

Upload to an Arduino Nano with:

```bash
/home/ttosi/.platformio/penv/bin/pio run --target upload
```

Required libraries are declared in `platformio.ini` and installed
automatically:

- PID
- MAX6675 library
- hd44780 with auto-detecting I2C backpack support

## PID Tuning

The initial PID constants are conservative starting values and require tuning
on the actual oven. Configuration values are in
[`include/Config.h`](include/Config.h).

Serial output uses `115200` baud and CSV format:

```text
ms,state,actual_c,target_c,output_percent,heater
```

Achieving `+/-1 C` depends on thermocouple placement, oven thermal lag, air
circulation, load, and PID tuning.

## Safety

- Never use the modified oven for food.
- Use suitable ventilation when processing resin, solder paste, or flux.
- Keep mains wiring physically separated from low-voltage controller wiring.
- Bond protective earth directly to every exposed metal part.
- Install a correctly rated main fuse, emergency heater-isolate switch,
  manual-reset high-limit thermostat, and one-shot thermal fuse.
- Mount the SSR outside the hot chamber on a correctly sized heat sink.
- Assume the SSR can fail closed and leave the heater continuously energized.
- Have mains wiring reviewed and assembled by someone qualified to work on line
  voltage.

Software shutdowns do not replace independent hardware thermal protection.

## Documentation

- [BOM.md](BOM.md): parts and safety hardware
- [PIN_MAPPING.md](PIN_MAPPING.md): complete low-voltage wiring
- [FIRMWARE.md](FIRMWARE.md): operation and tuning details
- [breadboard-tests/README.md](breadboard-tests/README.md): isolated component test firmware
- [IMPLEMENTATION_OUTLINE.md](IMPLEMENTATION_OUTLINE.md): design and validation plan
- [thermal-oven.kicad_sch](thermal-oven.kicad_sch): KiCad 10 schematic
- [thermal-oven.pdf](thermal-oven.pdf): rendered schematic
