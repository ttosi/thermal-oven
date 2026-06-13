# Arduino Nano Pin Mapping

| Nano Pin | Component | Component Pin / Function |
|---|---|---|
| `D2` | Rotary encoder | `CLK` |
| `D3` | Rotary encoder | `DT` |
| `D4` | Rotary encoder | `SW` push button |
| `D5` | SSR driver circuit | `1 kΩ` resistor to 2N2222 base |
| `D6` | MAX6675 module | `CS` |
| `D7` | MAX6675 module | `SCK` |
| `D8` | MAX6675 module | `SO` |
| `A4` | LCD1602 I2C adapter | `SDA` |
| `A5` | LCD1602 I2C adapter | `SCL` |
| `5V` | MAX6675, encoder, LCD, SSR input | `VCC` / positive supply |
| `GND` | MAX6675, encoder, LCD, SSR driver | Common low-voltage ground |

## SSR Driver

| Connection | Destination |
|---|---|
| Nano `D5` | `1 kΩ` resistor to 2N2222 base |
| 2N2222 base | `10 kΩ` resistor to `GND` |
| 2N2222 emitter | `GND` |
| 2N2222 collector | SSR DC input negative `-` |
| SSR DC input positive `+` | `5V` |

## Thermocouple

| Thermocouple Pin | MAX6675 Pin |
|---|---|
| Positive | `T+` |
| Negative | `T-` |

Verify the actual module pin labels and thermocouple polarity before powering
the system.
