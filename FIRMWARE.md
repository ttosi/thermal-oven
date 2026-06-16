# Firmware Operation

## Wiring

| Function | Nano Pin |
|---|---|
| Encoder `CLK` | `D2` |
| Encoder `DT` | `D3` |
| Encoder push button `SW` | `D4` |
| SSR transistor driver input | `D5` |
| MAX6675 `CS` | `D6` |
| MAX6675 `SCK` | `D7` |
| MAX6675 `SO` | `D8` |
| LCD adapter `SDA` | `A4` |
| LCD adapter `SCL` | `A5` |

The LCD driver auto-detects common I2C adapter addresses and backpack pin
mappings. Use the breadboard `i2c_scanner` and `lcd1602` tests when diagnosing
an adapter.

## Controls

- While stopped, rotate the encoder to select `60-260 C` in `5 C` increments.
- Press the encoder button to begin heating.
- While running, the LCD shows actual temperature, target temperature, and
  whether the SSR is currently commanding heat.
- Press the encoder button while running to immediately command the SSR off.
- Sensor failure or a reading at or above `280 C` latches a fault and commands
  the SSR off. Remove the cause and power-cycle the controller to clear it.

## PID Tuning

Initial PID constants and timing are in `include/Config.h`:

```cpp
PID_KP = 8.0
PID_KI = 0.25
PID_KD = 30.0
TEMPERATURE_SAMPLE_MS = 500
SSR_WINDOW_MS = 2000
```

These are conservative starting values, not qualified values for a particular
oven. The firmware logs CSV at `115200` baud:

```text
ms,state,actual_c,target_c,output_percent,heater
```

Tune with representative loads and record overshoot and steady-state error.
Achieving `+/-1 C` depends on sensor placement, oven thermal lag, air
circulation, load, and PID tuning.

The firmware safety checks do not replace the independent high-limit
thermostat, thermal fuse, fuse, emergency stop, and protective-earth hardware.
