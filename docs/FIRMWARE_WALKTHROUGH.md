# Understanding the final program

Read alongside firmware/test.c. This is the existing final source, not a new controller implementation.

1. `main()` first writes K1=K2=1, which commands both LOW-trigger relay modules OFF. `EA=0` disables interrupts through the IE register. It does NOT ground physical EA pin 31, which must remain HIGH for internal Flash execution.
2. `TMOD=0x11` selects internal-clock 16-bit Timer 0 and Timer 1. At 11.0592 MHz with 12 clocks per machine cycle, each timer increments at 921600 Hz. Timer 0 supplies short waits and millisecond delays; Timer 1 bounds ADC handshake waits to nominal 10 ms (65536−9216=0xDC00).
3. Software generates I2C on SDA/SCL. `send()` sends eight bits, releases SDA, and samples the ninth-clock acknowledgement. `probe()` checks the PCF8574/PCF8574A address ranges 0x20–0x27 and 0x38–0x3F. Exactly one response is required. The AT89S52 has no hardware I2C peripheral used here.
4. `backpack()`, `nibble()` and `lcd_byte()` convert a character or command into expander writes. Each LCD byte is split into two four-bit transfers, with an enable pulse for each. RS distinguishes commands from characters; RW stays LOW. Fixed waits replace LCD busy-flag reads.
5. `adc_read()` selects channel 0 or 1 using A; B and C are tied LOW. It pulses the shared ALE/START signal, checks EOC LOW, then lowers START and waits for EOC HIGH. The external NE555 drives the ADC clock throughout. P2=0xFF releases the quasi-bidirectional port for input before reading the eight ADC data lines.
6. After every channel switch the first conversion is discarded; the second is retained. A batch contains five retained values from each sensor, with a nominal 80 ms delay after each pair. All conversions, including discarded ones, have timeout checks.
7. `median5()` sorts five values and returns the middle one. This rejects up to two isolated extreme outliers. It cannot repair a wrong reference, sustained interference or a disconnected sensor producing a plausible voltage.
8. `control()` implements hysteresis and confirmation independently for fan and buzzer. An OFF fan needs raw temperature >=21 for three consecutive batches to turn ON. An ON fan needs raw <=18 for three consecutive batches to turn OFF. Gas uses >=140 and <=100. A batch that fails the relevant condition resets its counter. Values between thresholds retain the current state.
9. `temperature=(t*500UL+128UL)/256UL` converts the filtered temperature count to rounded Celsius for an assumed 5 V ADC reference and 10 mV/C LM35 gain. `unsigned long` arithmetic avoids intermediate multiplication overflow on C51. At that reference one count represents 500/256=1.953125 C. Integer display resolution is not measurement accuracy.
10. LCD R is filtered temperature raw count; G is filtered gas count. F/B indicate commanded states. K1=!fan_on and K2=!gas_on translate software ON into electrical LOW.

## Fault handling and limitations

ADC EOC timeout latches fan OFF/buzzer ON and displays the failed channel/phase. Reset is required after repair. Missing or ambiguous LCD address also stops normal operation with buzzer ON. Subsequent bus faults are checked at the end of the display update; do not claim instantaneous detection of every LCD fault. No robust sensor-disconnection detection or MQ2 warmup inhibition exists.

Sampling is polling-based and response time includes acquisition, display traffic and three qualifying batches. It is not a measured fixed-period interrupt scheduler. P3.4 and P3.5 remain GPIO because timers use their internal clocks, not external T0/T1 inputs.

The `LCD_ONLY` comment is obsolete: the source contains no `#ifdef LCD_ONLY`. Do not rely on that name to disable loads.
