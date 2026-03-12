
<img width="1344" height="768" alt="headshotmaster260312_0257_07" src="https://github.com/user-attachments/assets/d2bdc213-5171-4948-bbca-638319e9f301" />


# Wiring Instructions

## Arduino Pin Connections

| Arduino Pin | Component          | Notes                                |
|-------------|--------------------|--------------------------------------|
| 2           | Inside DHT22 data  | Use 4.7k–10k pull‑up resistor        |
| 10          | Outside DHT22 data | Use 4.7k–10k pull‑up resistor        |
| 3           | Heater relay (IN1) | Controls gas burner                   |
| 4           | Humidifier relay (IN2) |                               |
| 8           | Intake fan relay (IN1 of second module) |          |
| 9           | Exhaust fan relay (IN2 of second module) |         |
| 7           | Reset button       | Other leg to GND (internal pull‑up)  |
| A0          | LDR + 10k resistor | Voltage divider: LDR to 5V, resistor to GND, junction to A0 |
| 5V          | Sensor power       | Shared via RJ45 cables                |
| GND         | Common ground      | Shared via RJ45 cables                |

## Relay Module Connections (AC side)

- **Relay COM**: Connect to live (L) of AC mains (via breaker/fuse).
- **Relay NO (Normally Open)**: Connect to your device (heater, humidifier, fan).
- **Relay NC**: Not used.

**Important:** Keep all high‑voltage wiring inside an enclosure and follow local electrical codes.

## RJ45 Connector Pinout for 3‑meter Sensor Cables

We use CAT5 cable with RJ45 plugs on the box side and direct sensor connection on the other end.

| RJ45 Pin | Color (T568B) | Connection        | Destination               |
|----------|---------------|-------------------|---------------------------|
| 1        | White/Orange  | +5V               | DHT22 VCC (both)          |
| 2        | Orange        | Data 1            | Inside DHT22 data         |
| 3        | White/Green   | Data 2            | Outside DHT22 data        |
| 4        | Blue          | Ground            | DHT22 GND (both)          |
| 5        | White/Blue    | (spare)           | -                         |
| 6        | Green         | Ground            | DHT22 GND (both)          |
| 7        | White/Brown   | (spare)           | -                         |
| 8        | Brown         | (spare)           | -                         |

On the sensor end:
- Connect Orange/White (pin 1) to VCC.
- Connect Orange (pin 2) to data of inside sensor.
- Connect White/Green (pin 3) to data of outside sensor.
- Connect Blue and Green (pins 4 & 6) together to GND.
- Add a 4.7k–10k resistor between VCC and each data line at the sensor (pull‑up).

## LDR Mounting

Mount the LDR directly on the enclosure so it faces your grow lights. Use a 10k resistor between the LDR and GND, junction to A0.

## Button

Panel‑mount momentary push button: one leg to GND, the other to pin 7.

## Power

- 12V power supply → Arduino barrel jack (powers Arduino and relay coils via its 5V regulator).
- Alternatively, use a separate 5V supply for relay modules if needed.

**Always double‑check connections before applying power.**
