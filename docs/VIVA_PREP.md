# Viva preparation

## One-minute explanation

Our AT89S52 reads an LM35 temperature sensor and an MQ2 module through an external ADC0808. A separate NE555 provides the ADC clock. The controller displays estimated Celsius and gas ADC counts on an I2C LCD. Median filtering, separate ON/OFF thresholds and three-batch confirmation reduce unwanted switching. Two LOW-trigger relay channels switch the fan and active buzzer. The keypad was removed with faculty approval. We demonstrated hardware operation, but have not validated temperature accuracy, calibrated gas concentration, or completed Proteus simulation.

## Likely questions and short answers

1. **Why an external ADC?** The AT89S52 has no built-in ADC. ADC0808 converts the sensors' analog voltages into eight-bit values.
2. **How do you calculate Celsius?** LM35 produces nominally 10 mV/C. At a 5 V reference, one ADC count is 5/256 V, so temperature is approximately count × 500/256 C.
3. **Is a display of 025C accurate to 1 C?** No. The nominal ADC step alone is about 1.95 C, and reference, sensor and circuit errors add uncertainty.
4. **Why use NE555 when the MCU has a crystal?** The crystal clocks MCU execution. ADC0808 needs its own external clock; the NE555 supplies it.
5. **What do ALE, START, EOC and OE do?** ALE latches channel selection, START controls conversion start, EOC reports conversion status, and OE enables the digital output bus. Here ALE/START share a GPIO and OE stays HIGH.
6. **Why write 0xFF to P2 before reading?** It releases the quasi-bidirectional pins so the ADC can drive their input levels.
7. **Why is LOW relay ON?** This module's input circuit is active LOW; the MCU sinks its trigger current. The module drives the relay coil, and its contacts switch the load supply.
8. **Why not drive the motor from the MCU?** GPIO cannot supply motor current or tolerate its transients. Relay contacts switch a suitable external supply.
9. **What is hysteresis?** Different ON/OFF thresholds prevent repeated toggling near one threshold. Fan raw 21 turns ON, raw 18 turns OFF, after confirmation.
10. **Why a median rather than one reading?** The middle of five readings rejects isolated extreme values, but cannot remove sustained errors.
11. **Why three batches?** It requires persistence before switching, reducing short false triggers at the cost of extra response delay.
12. **What does gas 140 mean?** It is an ADC count threshold selected for demonstration, not 140 ppm or a certified safe limit.
13. **Are interrupts used?** No. Timer 0 provides delays and Timer 1 provides ADC timeouts using polling. Timer 2 is unused.
14. **Why can P3.4/P3.5 control relays?** Their alternate T0/T1 functions are unused because the timers count the internal machine-cycle clock.
15. **What if ADC conversion fails?** The program stops normal control, commands fan OFF/buzzer ON, displays a fault if possible and waits for reset.
16. **Why discard a conversion after channel changes?** It gives the analog input path additional settling opportunity; it does not guarantee removal of cross-channel interference.
17. **How is LCD I2C implemented?** Software toggles SDA/SCL and checks acknowledgements. The PCF8574 drives the LCD's four-bit parallel interface.
18. **What is the difference between compiling and flashing?** Keil translates C into machine code/HEX. The ISP programmer writes that HEX into the MCU's Flash.
19. **What changed since the proposal?** Keypad removed, thresholds fixed in firmware, LCD uses an I2C backpack, ADC uses P2, and sampling uses polling rather than interrupts.
20. **What would you improve?** Validate supplies and clock/relay logic margins, improve temperature resolution, calibrate sensing, add robust sensor-fault and warmup handling, and complete simulation and quantitative tests.

## Team rehearsal

Each person should trace one sensor voltage all the way to an output, explain the formula and one fault path, locate every assigned pin, and distinguish observations from unverified claims. Fill in actual individual contributions together; do not assign them retrospectively just for the report.
