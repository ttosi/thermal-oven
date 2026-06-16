# Thermal Oven BOM

## Existing Parts

| Qty | Item | Planned Part / Link | Schematic Reference | Notes |
|---:|---|---|---|---|
| 1 | Conventional toaster oven | Existing appliance | J8, J9 | Heating elements and grounded metal chassis |
| 1 | Arduino Nano ATmega328P | Generic Nano, new bootloader | A1 | 5 V controller |
| 1 | MAX6675 module and K-type thermocouple | [Amazon](https://a.co/d/00btdsHs) | A2, J2 | Module pins assumed `GND`, `VCC`, `SCK`, `CS`, `SO`; verify before wiring |
| 1 | SSR-25DA solid-state relay module | [Amazon](https://a.co/d/06sf6czt) | A4 | DC input, AC output; mount to a correctly sized heat sink |
| 1 | Rotary encoder with push button | [Amazon](https://a.co/d/00znrNOg) | A3 | Module pins assumed `GND`, `VCC`, `SW`, `DT`, `CLK`; verify before wiring |
| 1 | LCD1602 16x2 character display | [SunFounder wiki](http://wiki.sunfounder.cc/index.php?title=LCD1602_Module) | DS1 | Driven through the I2C adapter |
| 1 | LCD1602 I2C adapter extended board | [Amazon](https://a.co/d/0b7io4Lv) | A5 | Address and backpack pin mapping are auto-detected; verify with the breadboard diagnostics |

## Required Controller Electronics

| Qty | Item | Suggested Rating / Value | Schematic Reference | Notes |
|---:|---|---|---|---|
| 1 | NPN transistor | 2N2222, PN2222A, or equivalent | Q1 | SSR input driver |
| 1 | Base resistor | 1 kohm | R1 | Nano to Q1 base |
| 1 | Base pulldown resistor | 10 kohm | R2 | Keeps SSR off during reset |
| 2 | Decoupling capacitor | 100 nF ceramic | C1, C2 | Place close to Nano and MAX6675 supply pins |
| 1 | Bulk capacitor | 10 uF electrolytic | C3 | Place near the LCD/I2C adapter supply connection |
| 1 | Buzzer | 5 V active buzzer | BZ1 | Audible completion/fault indication |
| 1 | Isolated regulated power supply | Approved enclosed 5 V DC supply, at least 1 A | PS1 | Keep mains and SELV wiring separated |

## Required Mains Safety Hardware

Final voltage/current ratings must be selected from the toaster oven nameplate
and applicable local electrical requirements.

| Qty | Item | Requirement | Schematic Reference |
|---:|---|---|---|
| 1 | Main fuse and holder | Sized for oven load and wiring | F1 |
| 1 | Two-pole main switch | Rated for mains voltage and oven current | SW1 |
| 1 | Latching emergency-stop / heater-isolate switch | Normally closed, rated for heater current or used with a suitable contactor | SW2 |
| 1 | Manual-reset high-limit thermostat | Normally closed, independent of Arduino | TH1 |
| 1 | One-shot thermal fuse | Independently interrupts heater on over-temperature | F2 |
| 1 | SSR heat sink and thermal compound | Sized using verified SSR dissipation and measured temperature | A4 |
| 1 set | Mains terminals, wire, crimp terminals, barriers, and strain relief | Finger-safe and temperature/current rated | J7-J9 |
| 1 | Protective-earth hardware | Dedicated chassis bonding stud and hardware | J7 |

## Open Part Selections

- Exact Nano board/module pin labeling
- Oven nameplate voltage, power, and current
- High-limit thermostat and thermal-fuse trip temperatures
- Approved 5 V power supply

## Firmware Pin Map

| Function | Nano Pin |
|---|---|
| Encoder `CLK` | `D2` |
| Encoder `DT` | `D3` |
| Encoder push button `SW` | `D4` |
| SSR driver | `D5` |
| MAX6675 `CS` | `D6` |
| MAX6675 `SCK` | `D7` |
| MAX6675 `SO` | `D8` |
| LCD I2C `SDA` | `A4` |
| LCD I2C `SCL` | `A5` |
