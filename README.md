# MOG-BLEJAMMER

## WARNING: Using jammers is illegal and you can get in trouble.

---

## Current supported devices: ESP32WROOM32U

---

## REQUIRED:
- ESP32WROOM32U
- Jumper wires
- x1 NRF24L01 (or pa+lna)
- Battery and battery module and a switch (optional)

## PINOUT

| NRF24L01 Pins | ESP32 Pins |
|--------------|-----------|
| VCC          | 3.3V      |
| GND          | GND       |
| CSN          | GPIO5     |
| CE           | GPIO4     |
| SCK          | GPIO18    |
| MOSI         | GPIO23    |
| MISO         | GPIO19    |

## HOW TO USE:

- Download ArduinoIDE
- Go to Tools>Board>BoardManager and download esp32 by Espressif Systems. Go to File>Preferences>Additional Board Manager URLs and paste ``https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json``
- 
