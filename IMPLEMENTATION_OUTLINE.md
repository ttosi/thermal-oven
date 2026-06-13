# PID Toaster Oven Implementation Outline

## 1. Scope and design goals

Build a benchtop thermal-processing oven with two explicitly separate operating
modes:

1. **SMD reflow**: follow a solder-paste-specific temperature profile measured
   at the PCB.
2. **Resin treatment**: hold a resin-vendor-specific temperature for a defined
   time. Heat is not a universal substitute for UV post-curing; use the resin
   manufacturer's specified wavelength, time, and temperature.

This oven must never be used for food after processing resin, solder paste, or
flux. Operate it with suitable ventilation and follow each material's SDS.

## 2. BOM assessment

### Existing parts

- Conventional toaster oven
- Arduino Nano ATmega328P
- MAX6675 module and K-type thermocouple
- SSR-25DA AC solid-state relay
- LCD1602 display with I2C adapter
- Rotary encoder with push button

These parts are enough to prototype temperature measurement, user input, and
time-proportioned heater control. They are not enough for a safe finished oven.

### Required additions

- Properly rated branch fuse or circuit breaker for the oven nameplate current
- Independent, normally closed, manual-reset high-limit thermostat in series
  with the heater
- One-shot thermal fuse as a final over-temperature backup
- Main power switch and a prominent latching emergency-stop or heater-isolate
  switch
- SSR heat sink, thermal compound, and guarded mounting
- Finger-safe mains terminals, high-temperature wire, crimp terminals, strain
  relief, insulating barriers, and a grounded metal enclosure
- Isolated, enclosed, approved low-voltage DC supply for the controller
- Buzzer and clear heater-active indicator
- Known-value 0.1 uF bypass capacitor at the MAX6675 module if the module does
  not already provide adequate local decoupling

### Strongly recommended additions

- A second thermocouple and reader, or an independent thermocouple logger, for
  validating temperature uniformity and detecting sensor/control faults
- A removable board-level thermocouple for reflow and a fixed chamber/load
  thermocouple for resin mode
- Verify the LCD1602 I2C adapter address and connector pinout before wiring
- A heat-rated circulation fan if testing shows unacceptable hot/cold spots
- Door switch if the desired behavior is to disable heat whenever the door is
  open

## 3. Safety architecture

Mains wiring and safety hardware should be reviewed and assembled by someone
qualified to work on line voltage. Keep all mains circuitry physically
separated from the controller and user interface.

Recommended heater power path:

```text
Line
  -> main fuse/breaker
  -> emergency-stop / heater-isolate switch
  -> independent manual-reset high-limit thermostat
  -> one-shot thermal fuse
  -> SSR output
  -> original oven thermostat/high-limit, if suitable and retained
  -> heating elements
Neutral
```

Bond protective earth directly to every exposed metal part. Do not switch or
fuse the protective-earth conductor.

The SSR is only the normal control device. It must not be the only way to stop
heating. Solid-state relay output devices commonly fail shorted, leaving the
heater on even when the controller requests off.

Select the fuse, switches, terminals, wire, SSR, and heat sink from the oven's
measured/nameplate voltage and current, not from the SSR's advertised "25 A"
alone. Verify the actual SSR's datasheet and provenance. Mount the SSR outside
the hot chamber and measure its heat-sink temperature during a worst-case run.

## 4. Sensor and mechanical setup

1. Preserve the oven's insulation, grounded chassis, and useful original
   thermal protections.
2. Put the Nano, MAX6675, SSR, display, and low-voltage supply outside the hot
   chamber.
3. Keep thermocouple wiring away from mains and heater wiring. Use proper
   K-type extension wire/connectors; do not splice it with ordinary copper in a
   temperature gradient.
4. Mount the MAX6675 away from the oven wall, SSR, regulator, and other heat
   sources because its board temperature is used for cold-junction
   compensation.
5. For reflow, attach the control probe to a representative sacrificial PCB or
   test coupon near the work. Chamber-air temperature alone does not reliably
   represent component or joint temperature.
6. For resin mode, place the probe consistently near the part without touching
   a heating element or metal wall.
7. Characterize the empty oven and representative loads with probes at several
   locations before processing valuable parts.

## 5. Firmware architecture

Use a non-blocking state machine. The heater output must default off during
startup, reset, sensor failure, and any unexpected state.

### Suggested modules

- `temperature`: reads MAX6675 no faster than its conversion interval, validates
  readings, filters obvious noise without hiding fast faults
- `heater`: owns the SSR pin and time-proportioning window
- `controller`: PID calculation, output limits, anti-windup, and tuning values
- `profile`: recipe stages and target-temperature interpolation
- `safety`: independent software checks and latched fault handling
- `ui`: encoder, display, buzzer, start/cancel confirmation
- `logging`: serial CSV output for tuning and run validation

### Top-level states

```text
BOOT_SELF_TEST -> IDLE -> PROFILE_SELECT -> READY
READY -> PREHEAT/RAMP -> SOAK -> PEAK/HOLD -> COOL -> COMPLETE
any active state -> FAULT_LATCHED
any active state -> CANCELLED
```

`FAULT_LATCHED` requires user acknowledgement and a power cycle or explicit
reset after the cause is removed. It must never automatically resume heating.

### Recipe model

Represent recipes as ordered segments:

```cpp
struct Segment {
  float targetC;
  uint32_t durationMs;
  SegmentMode mode; // RAMP, HOLD, or COOL
};
```

Interpolate the setpoint during `RAMP`; hold it during `HOLD`; force heater off
during passive `COOL`. Store built-in conservative test recipes in flash.
Only add EEPROM editing after the core control and safety behavior is tested.

### Heater control

- Use a zero-cross AC SSR and time-proportioned control, not high-frequency PWM.
- Start with a 1 to 2 second output window and convert PID output percentage to
  on-time in that window.
- Use a sample period compatible with the MAX6675's approximately 0.22 second
  worst-case conversion time; 500 ms is a practical starting point.
- Clamp output and use anti-windup. Reduce or inhibit output as the measured
  temperature approaches the target to limit overshoot.
- Expect separate PID tuning for low-temperature resin holds and fast reflow.
- Because there is no active cooling actuator, the controller can only turn
  heat off during cooling and prompt the operator to open the door when safe.

### Software safety checks

Latch a fault and command SSR off when any of these occurs:

- Thermocouple-open indication, invalid reading, or stale reading
- Temperature above a hard firmware limit
- Temperature rises while SSR command has remained off long enough to suggest
  an SSR stuck on
- Temperature fails to rise while high output is commanded, suggesting a
  detached sensor or heater fault
- Implausible rate of temperature rise or fall
- Profile stage exceeds its maximum allowed time
- Watchdog reset or internal state corruption

Software checks supplement, but do not replace, the independent high-limit
thermostat and thermal fuse.

## 6. Recipe strategy

### SMD reflow

Do not create one universal "lead-free" or "leaded" recipe. Start from the
specific solder paste manufacturer's reflow profile, then verify that every
component on the PCB permits that peak temperature and time-above-liquidus.

The profile engine should support:

- Preheat/ramp with a maximum rise rate
- Soak range and duration
- Ramp to peak
- Peak-temperature ceiling
- Time above solder liquidus
- Maximum cooling rate, to the extent passive cooling can control it

Log actual measured temperature and calculate achieved ramp rates, peak, soak
time, and time above liquidus after every development run.

### Resin parts

Create a separate recipe per resin product and part geometry. Record:

- Vendor-specified washing/drying steps
- Required light wavelength and exposure, if any
- Hold temperature and duration
- Support/fixture method to prevent warping
- Allowed cooling method

Use the resin's current technical data sheet and SDS. Excess time or
temperature can cause brittleness, shrinkage, or warping.

## 7. Build and validation phases

### Phase 1: Bench prototype, no mains switching

- Wire Nano, MAX6675, thermocouple, encoder, display, buzzer, and SSR input.
- Replace the heater with an LED on the SSR-control output.
- Implement sensor reads, UI, serial logging, state machine, and all fault
  transitions.
- Verify that reset, unplugged probe, corrupt recipe, and cancel always command
  heater off.

Exit criterion: automated tests cover profile interpolation and faults; a
simulated run produces the expected state transitions and output percentages.

### Phase 2: Safe power assembly

- Measure oven nameplate voltage, wattage, and current.
- Draw and review the mains schematic.
- Install fuse, earth bonding, independent high-limit devices, SSR heat sink,
  barriers, strain relief, and enclosed low-voltage supply.
- Perform continuity, protective-earth, insulation, and no-load checks before
  energizing.

Exit criterion: the heater can be physically disconnected by the emergency
stop and high-limit devices regardless of Nano or SSR input state.

### Phase 3: Characterize the oven

- Run at low power first with no valuable load.
- Log temperature with multiple independent probes.
- Determine heat-up rate, overshoot, cool-down rate, SSR heat-sink temperature,
  and spatial uniformity.
- Adjust sensor location, insulation, shielding, or circulation based on data.

Exit criterion: no safety trips or overheated electronics, and measured
temperature spread is acceptable for the intended processes.

### Phase 4: Tune control

- Tune low-temperature hold behavior first.
- Tune the fast reflow profile separately using sacrificial PCB coupons.
- Add conservative output limiting/feed-forward if the oven's thermal lag
  causes overshoot that PID tuning alone cannot avoid.

Exit criterion: several consecutive runs meet the selected recipe's measured
limits with representative loads.

### Phase 5: Process qualification

- Reflow inexpensive test boards using the exact intended solder paste.
- Inspect joints for opens, bridges, tombstoning, and overheating.
- Process disposable resin test coupons using the exact intended resin.
- Record recipe revision, load, probe placement, actual profile, and results.

Exit criterion: repeatable results and documented safe operating procedure.

## 8. Initial firmware milestones

1. Replace the placeholder `src/main.cpp` with pin definitions, safe startup,
   MAX6675 reading, and serial output.
2. Add the non-blocking state machine and profile interpolation with a simulated
   heater.
3. Add encoder/display UI and latched faults.
4. Add time-proportioned SSR output and watchdog.
5. Add serial CSV logs and post-run profile metrics.
6. Add tests for recipes, state transitions, timeouts, and fault behavior.
7. Only then connect the SSR output to the oven heater power circuit.

## 9. Research basis

- Analog Devices, **MAX6675 datasheet**:
  <https://www.analog.com/media/en/technical-documentation/data-sheets/MAX6675.pdf>
  - 0.25 C resolution, approximately 0.22 s maximum conversion time, open-probe
    detection, cold-junction placement guidance, and noise precautions.
- Omron, **Technical Explanation for Solid-state Relays**:
  <https://www.ia.omron.com/data_pdf/guide/18/ssr_tg_e_9_2.pdf>
  - Heater control using seconds-scale switching, heat-sink/derating
    requirements, and fail-safe guidance because most output-device faults are
    short-circuit failures that prevent the load from turning off.
- Erickson and Durfee, **Reflow Soldering of Surface Mount Electronic
  Components in a Laboratory**:
  <https://arxiv.org/abs/0901.0136>
  - Demonstrates toaster-oven reflow and, importantly, characterizes a PCB with
    five thermocouples rather than assuming one air-temperature reading is
    representative.
- Formlabs, **How to Post-Cure Your Resin 3D Prints**:
  <https://formlabs.com/blog/how-to-post-cure-3d-prints/>
  - Post-cure time and temperature are resin- and geometry-specific; excessive
    treatment can cause brittleness or warping; light wavelength matters.
