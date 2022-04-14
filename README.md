# DwellerFirmware

Arduino Uno firmware for the box with security features.

Components:

- Arduino Uno
- 10mm RGB Led Module
- Triple Axis Accelerometer Breakout - ADXL345 - GY-291
- Wireless NRF24L01+ 2.4Ghz Transceiver Module
- RFID NFC Module RC522
- Potentiometer YL-83
- Buzzer
- 3 x 10mm LDR
- 100Ω 1/4W Resistor
- 100kΩ 1/4W Resistor

## Compiling

Presently, two macros control certain debug and development options for the Dweller firmware,
these are off by default but can be toggled on by uncommenting their respective lines, or by
passing them to the avr-compiler if used manually.

- `SILENT` can be used to turn off any sound coming from the Dweller.
- `DEBUG` can be used to turn Serial prints on, which are not on in production.
