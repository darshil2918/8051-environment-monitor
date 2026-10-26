# 8051 Environmental Monitor

College hardware demonstration using an AT89S52, ADC0808, LM35 and MQ2. It displays estimated temperature in Celsius and gas ADC counts, operates a fan through relay K1, and operates an active buzzer through relay K2.

## Status

The team reports successful LCD operation, temperature-triggered fan operation and gas-triggered buzzer operation. These are qualitative observations, not calibrated accuracy or gas-safety validation. Proteus work is incomplete and deferred. The keypad was removed with faculty approval.

## Files

- `firmware/test.c`: unchanged final integrated Keil C51 source, including the Celsius display.
- `firmware/build.ps1`: build using a locally installed Keil C51 toolchain.
- `releases/environment_monitor.hex`: existing final HEX, copied without rebuilding.
- `docs/WIRING.md`: actual application pin map.
- `docs/FIRMWARE_WALKTHROUGH.md`: how the code works.
- `docs/TEST_RESULTS.md`: evidence and remaining checks.
- `docs/VIVA_PREP.md`: questions and short answers.
- `docs/REPORT_DRAFT.md`: report draft; missing measurements and administrative details are marked.
- Wiring PDF: connection reference, including proposed Proteus equivalents; it does not establish that simulation was completed.

## Build and program

Use Keil C51 for AT89S52, 11.0592 MHz, standard 12-clock operation. In PowerShell, from this repository:

```powershell
& .\firmware\build.ps1 -KeilRoot 'C:\Keil_v5\C51'
```

The output is `firmware/environment_monitor/environment_monitor.hex`. The script obtains STARTUP.A51 and libraries from your own Keil installation; compiler and library files are not included here. Alternatively, create a Keil AT89S52 project, add test.c and the standard startup file, and enable Create HEX File.

Keil compiles the program; the external ISP programmer transfers the HEX. The working setup used ProgISP 1.72, AT89S52 selection, signature 1E 52 06, erase/program/verify, and individual jumper wires replacing a faulty supplied cable. Verify the programmer's own connector labels before connecting it; connector layouts are not universal. RS232 is not the AT89S52's native ISP interface.

## Operation

K1 and K2 are active LOW. Both start OFF. Five retained readings per channel are median-filtered. Three consecutive qualifying batches are required to change each output. Fan thresholds are raw 21 ON and raw 18 OFF (approximately 41.02 and 35.16 C at a 5 V reference). Gas thresholds are raw 140 ON and raw 100 OFF; these are demonstration settings, not ppm.

LCD line 1 shows `Temp:025C G:090`; line 2 shows temperature raw count R and commanded fan/buzzer states F/B. These states are not physical feedback. ADC handshake errors latch fan OFF and buzzer ON until reset. LCD communication faults also latch that output state when detected.

## Important limitations

The nominal temperature step is about 1.95 C per ADC count. Earlier room-temperature and cross-channel readings were suspicious; calibrated accuracy remains unverified. Filtering does not fix wiring, reference, supply or sensor errors. MQ2 warmup is not inhibited in firmware. NE555 output HIGH margin and direct relay-input drive margins require measurement. This is an educational prototype, not a certified gas alarm.

The source's old `DEFINE(LCD_ONLY)` comment is stale: this final source has no such conditional and defining it does NOT disable relay outputs. The working source is preserved unchanged.
