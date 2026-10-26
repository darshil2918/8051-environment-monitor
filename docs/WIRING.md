# Final application wiring

AT89S52 DIP-40, 11.0592 MHz. All logic uses a common ground and nominal regulated 5 V. The NSK board supplies the MCU crystal/reset circuitry. Its 12 V adapter input is not a 5 V peripheral supply. Do not parallel independent regulator outputs.

| MCU pin | Signal | Connection |
|---|---|---|
| 3 | P1.2 | LCD backpack SDA |
| 4 | P1.3 | LCD backpack SCL |
| 12 | P3.2 | ADC pin 25, address A |
| 13 | P3.3 | ADC pin 6 START and pin 22 ALE |
| 14 | P3.4 | Relay module IN1, fan |
| 15 | P3.5 | Relay module IN2, buzzer |
| 16 | P3.6 | ADC pin 7 EOC |
| 21 | P2.0 | ADC pin 17 D0 |
| 22 | P2.1 | ADC pin 14 D1 |
| 23 | P2.2 | ADC pin 15 D2 |
| 24 | P2.3 | ADC pin 8 D3 |
| 25 | P2.4 | ADC pin 18 D4 |
| 26 | P2.5 | ADC pin 19 D5 |
| 27 | P2.6 | ADC pin 20 D6 |
| 28 | P2.7 | ADC pin 21 D7 |

There are 15 distinct application GPIO pins, with no duplicate assignments. ISP reserves P1.5/pin 6 MOSI, P1.6/pin 7 MISO, P1.7/pin 8 SCK and pin 9 RST. MCU pin 20 is ground, 40 is +5 V and physical EA pin 31 is HIGH for internal program memory. Crystal pins are 18/19. Unlisted GPIO are unused by this application; onboard circuitry may still connect to them.

## ADC0808 remaining connections

| ADC pin(s) | Connection |
|---|---|
| 1, 2, 3, 4, 5, 28 | Unused IN3..IN7 and IN2, ground |
| 9 | OE, +5 V; data bus dedicated to ADC |
| 10 | Clock from NE555 pin 3 |
| 11, 12 | VCC and VREF+, +5 V |
| 13, 16 | Ground and VREF−, ground |
| 23, 24 | Address C and B, ground |
| 26 | IN0, LM35 output |
| 27 | IN1, MQ2 module AO |

Use 100 nF decoupling near ADC pins 11/13 and 12/16. Sensor supply/ground are shared with the analog reference ground. MQ2 DO is unused. Identify the actual LM35 package orientation before connecting its supply; do not infer pin order from an arbitrary photo.

## NE555 astable

Pins 1 ground, 8 +5 V, 4 +5 V, 2 joined to 6. RA=10 kΩ from +5 V to pin 7; RB=10 kΩ from pin 7 to pins 2/6. Timing capacitor 1 nF from pins 2/6 to ground. Pin 5 has 10 nF to ground. Pin 3 drives ADC pin 10. Add 100 nF across supply pins 8/1 and local bulk capacitance (e.g. 10 µF). Nominal frequency is about 48 kHz; this is separate from the MCU crystal. Actual frequency and HIGH voltage margin have not been instrument-verified.

## LCD and loads

LCD backpack VCC=5 V, GND=common ground. SDA/SCL require pull-ups to 5 V; check existing backpack pull-ups before adding any. Contrast is set with the backpack potentiometer. Code assumes PCF8574 P0=RS, P1=RW, P2=E, P3=backlight, P4..7=D4..7.

Relay module VCC=5 V and GND=common ground. K1 COM connects to the motor's 5 V supply, K1 NO to motor positive; motor negative returns to its supply ground. K2 COM connects to buzzer 5 V, K2 NO to buzzer positive; buzzer negative returns to ground. NC contacts unused. The relay contacts switch load power; the MCU only drives module inputs. Motor suppression diode (cathode to positive) and motor decoupling are recommended, but installation was not confirmed.

Each module input reportedly sinks approximately 1.8 mA when grounded. This is above the MCU's 1.6 mA guaranteed LOW-voltage test point; successful switching is not a guarantee of electrical margin. Measure actual input LOW/HIGH levels in the assembled circuit.
