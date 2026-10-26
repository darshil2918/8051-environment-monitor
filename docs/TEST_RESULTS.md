# Test evidence and remaining validation

Status date: 27 October 2026. Hardware observations below were reported by the project owner; they were not independently measured by the documentation author.

| Test | Available evidence | Conclusion |
|---|---|---|
| Relay-only sequence | Both channels switched through the requested approximately 10-second states | Functional switching reported |
| ISP upload | Supplied connection cable failed; individual jumpers allowed programming | Upload method worked in that setup |
| LCD | Initially blank, then worked after contrast adjustment | Display operation reported |
| ADC channels | Initial EOC error resolved; values changed with sensor stimulus | Conversion activity reported |
| Integrated gas response | Buzzer activated during reported exposure | Demonstration threshold response reported, no concentration calibration |
| Integrated temperature response | Fan activated when temperature increased | Demonstration response reported, no accuracy curve |
| Celsius display | User confirmed final display change worked | Celsius-labelled final version accepted |
| Keypad | Test unsuccessful; faculty approved removal | Not part of final design |
| Proteus | Partial schematic only | Not simulated or validated |

Earlier reported room-temperature raw 006 corresponds to approximately 12 C at a 5 V reference. Gas testing also reportedly disturbed the temperature reading. Later functional success does not resolve these accuracy concerns by itself.

## Record before making quantitative claims

- Measure ADC VREF+ relative to VREF− and LM35 voltage at both sensor and ADC input.
- Compare displayed temperature with a reference thermometer at several stable points; record actual values, not expected values.
- Record relay input HIGH/LOW voltages and supply stability during load switching.
- Measure NE555 clock frequency and HIGH/LOW levels with an oscilloscope if available.
- Record startup drift and baseline gas counts; do not convert counts to ppm without calibration.
- Measure response time and repeatability if reporting numeric performance.
- Record each member's actual contribution and obtain faculty guidance on the deferred simulation requirement.

No accuracy percentage, ppm threshold, guaranteed detection distance, exact alarm delay or simulated result is claimed.
