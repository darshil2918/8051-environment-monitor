# Environmental Safety Monitor Using an 8051 Microcontroller

Final hardware report draft — 27 October 2026

Team: Darshil Y R, Shashvat Sharma, Zameer Zaad. Department: Electrical and Electronics Engineering, MIT Manipal. Faculty, section, submission date and required final-report format: **to be confirmed**. Registration numbers are omitted from this public repository.

## 1. Problem statement

Develop a college demonstration that senses temperature and a gas-sensitive analog signal, displays their values, and controls a fan and audible indicator using an 8051-family microcontroller. The prototype demonstrates sensing and control; it is not a certified life-safety system.

## 2. Introduction

The AT89S52 coordinates sensor acquisition, display and relay control. Because it has no internal ADC, an ADC0808 digitizes the LM35 output and MQ2 module analog output. An NE555 astable supplies the ADC clock. A 16×2 LCD with an I2C backpack reduces display wiring. Two relay channels switch a 5 V toy motor and active buzzer.

## 3. Objectives and final scope

- Acquire two analog sensor channels and display estimated Celsius and raw gas counts.
- Activate the fan and buzzer using independent threshold rules.
- Reduce transient switching through median filtering, hysteresis and confirmation.
- Detect ADC handshake failures and certain LCD communication failures.
- Demonstrate hardware operation and document limitations honestly.

The initial keypad-based threshold entry was removed with faculty approval. Final thresholds are compile-time constants. The final hardware uses an AT89S52, an I2C LCD and polling-based acquisition. Earlier proposal descriptions of a parallel LCD, ADC on Port 1 and interrupt sampling do not describe this implementation.

## 4. Engineering systems, variables and parameters

The MCU runs at 11.0592 MHz in standard 12-clock operation. ADC0808 uses nominal 0–5 V reference and eight-bit conversion. LM35 nominal sensitivity is 10 mV/C. MQ2 AO is a gas-sensitive voltage; no concentration calibration was performed. Relay channels are active LOW.

Temperature estimate: T ≈ N × 5/(256 × 0.01) = N × 1.953125 C. The firmware rounds the result for display. Fan thresholds are raw 21 ON and 18 OFF, approximately 41.02 C and 35.16 C at nominal reference. Gas thresholds are raw 140 ON and 100 OFF. Three qualifying batches are required for either transition.

## 5. Existing approaches and selected method

Analog comparators can provide threshold switching with few parts, while microcontrollers with integrated ADCs can reduce component count. This project uses the 8051 and external ADC to demonstrate parallel interfacing, timed handshaking, software serial communication and load control within the laboratory context. The added wiring and low temperature resolution are tradeoffs.

## 6. Prototype and working principle

The signal path is: sensors → ADC0808 → AT89S52 → LCD and relay module → fan/buzzer. NE555 provides the ADC clock independently of the MCU crystal. The final pin connections are recorded in WIRING.md and the accompanying connection PDF. **Insert a reviewed circuit diagram and final assembled prototype photograph before submission; the existing Proteus project is incomplete.**

At startup, both relays are commanded OFF. The firmware detects the LCD backpack address and initializes the LCD. It acquires two conversions per channel, discards the first after switching and retains the second. After collecting five retained readings for each channel, it selects each median. Independent hysteresis and confirmation counters determine outputs. The LCD shows Celsius, raw gas, raw temperature and commanded output states. This cycle repeats.

If ADC EOC does not reach the expected state within a nominal 10 ms timeout, the firmware latches fan OFF/buzzer ON and requests reset. Missing or failed LCD communication also causes a latched output fault state when detected. This is a demonstration policy rather than a validated safety response.

Algorithm: initialize → detect/init LCD → acquire paired sensor batch → check handshake faults → calculate medians → update threshold counters/relays → convert temperature → display → check LCD bus fault → repeat. Fault branches latch until reset.

## 7. Implementation and results

Keil C51 was used to build the firmware. An external ISP programmer and ProgISP were used to load the AT89S52. A faulty supplied connection cable was replaced with individual jumpers. The LCD initially appeared blank and worked after contrast adjustment. ADC handshake errors were subsequently resolved, and changing readings were observed on both channels.

The team reported successful gas-triggered buzzer operation, temperature-triggered fan operation and Celsius display operation. These are qualitative observations. TEST_RESULTS.md records the evidence boundaries and remaining measurements. No accuracy percentage, calibrated ppm, numeric response-time result or successful Proteus simulation is claimed.

**Before submission:** add actual test measurements, dated hardware photographs, a reviewed circuit diagram and each member's true contribution. The lab manual includes simulation results in project evaluation, so confirm the treatment of deferred simulation with faculty.

## 8. Limitations

Temperature quantization is approximately 1.95 C per count at the nominal reference. Earlier room-temperature raw 006 and temperature disturbances during gas testing require investigation; later successful switching alone does not validate accuracy. MQ2 baseline drift and warmup may affect alarms, and gas thresholds are not concentration limits. Filtering cannot correct sustained supply, reference or wiring errors. Direct relay input current and NE555 HIGH-level margins remain to be checked. Sensor disconnection is not comprehensively detected. Simulation is incomplete.

## 9. Conclusion

The prototype demonstrates a complete 8051-based sensing, display and relay-control path. The owner reports successful qualitative hardware operation. Further electrical checks, sensor calibration and quantitative testing are required before claiming measurement accuracy or dependable environmental hazard detection.

## 10. References

- Microchip, [AT89S52 datasheet](https://ww1.microchip.com/downloads/en/devicedoc/doc1919.pdf).
- Texas Instruments, [ADC0808/ADC0809 datasheet](https://www.ti.com/lit/ds/symlink/adc0808-n.pdf).
- Texas Instruments, [LM35 datasheet](https://www.ti.com/lit/ds/symlink/lm35.pdf).
- Texas Instruments, [NE555 product documentation](https://www.ti.com/product/NE555).
- Supplied Microcontroller Lab Manual (2026) and mid-term report format, consulted locally and not redistributed here.
- Final source: ../firmware/test.c; firmware explanation: FIRMWARE_WALKTHROUGH.md.
