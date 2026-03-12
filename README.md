# Automated Greenhouse VPD Controller

An Arduino-based system that maintains optimal Vapor Pressure Deficit (VPD) for flower production. Features smart ventilation using outside air, day/night VPD targets that ramp over a 60‑day crop cycle, and energy‑saving on/off control.

## Features

- **VPD‑based control** – Heater, humidifier, intake/exhaust fans maintain VPD within target bands.
- **60‑day ramping** – Daytime VPD target gradually increases from 0.6–1.0 kPa (day 0) to 1.0–1.4 kPa (day 60+).
- **Day/night separation** – LDR sensor detects light; nighttime target fixed at 0.4–0.8 kPa.
- **Smart ventilation** – Fans run only when outside air (10–30°C) improves VPD or temperature.
- **Minimum temperature safety** – Heater overrides VPD control to prevent cold damage (night <16°C, day <20°C).
- **Crop reset button** – Resets day counter to 0 for new cycle, saved in EEPROM.
- **Power‑saving** – Devices only activate when conditions leave the allowed fluctuation band (±0.4 kPa).

## Hardware Required

| Component                  | Quantity | Approx. Cost (AliExpress) |
|----------------------------|----------|---------------------------|
| Arduino Uno (ATmega328P)   | 1        | $4                        |
| DHT22 temperature/humidity sensors | 2  | $4                        |
| 2‑channel 5V relay modules | 2        | $4 each                   |
| LDR + 10k resistor         | 1 set    | $1                        |
| Push button                | 1        | $0.50                     |
| Breadboard + jumper wires  | 1 pack   | $3                        |
| Project enclosure          | 1        | $8                        |
| 12V power supply           | 1        | $10                       |
| RJ45 jacks & CAT5 cable    | as needed| $10                       |
| **Total**                  |          | **~$45**                  |

Plus a gas burner (e.g., Titan Controls Ares) for winter heating + CO₂.

## Wiring Diagram

See [wiring.md](wiring.md) for detailed pin connections and RJ45 pinout for 3‑meter sensor cables.

## How to Use

1. Assemble the hardware according to wiring.md.
2. Install the DHT sensor library via Arduino Library Manager.
3. Upload `greenhouse_controller.ino` to your Arduino.
4. Adjust `LDR_THRESHOLD` in the code after testing with your lights.
5. Press the reset button at the start of each crop cycle.

The system runs completely autonomously. No display or user input needed after setup.

## Code Overview

- **Inside/outside DHT22** read every 2 seconds.
- **LDR** determines day/night.
- **Day counter** increments every 24h, stored in EEPROM.
- **VPD calculation** using standard formula.
- **Target bands** chosen based on day/night and crop day.
- **Actuator logic**:
  - Heater: ON if VPD below band (and temp < max) OR temp below minimum.
  - Humidifier: ON if VPD above band (and humidity < 80%).
  - Fans: ON if outside air (10–30°C) helps VPD or temperature.
- **Deadband of 0.4 kPa** – no action when VPD inside band.

## Customization

Edit the constants at the top of the `.ino` file to change:
- Temperature limits (`T_MIN_DAY`, `T_MAX_DAY`, `T_MIN_NIGHT`, `T_MAX_NIGHT`)
- VPD ranges (`VPD_DAY_START_LOW`, `VPD_DAY_END_HIGH`, etc.)
- Safe outside temperature range (`OUT_TEMP_MIN`, `OUT_TEMP_MAX`)
- LDR threshold after testing

## License

This project is open source under the MIT License.
