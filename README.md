# 7Semi One Wire Library

Arduino library for the One-Wire temperature and humidity probe. This version ports the supplied **working** `temp_humidity.h` reference, including its nine exact command frames, discovered ROM substitution, initialization order, and command 8 + command 9 measurement cycle.

## Install
Arduino IDE > Sketch > Include Library > Add .ZIP Library. Install the **OneWire** library dependency. Open File > Examples > 7Semi One Wire Library > BasicReading. Select Arduino UNO, upload, and open Serial Monitor at **9600 baud**.

## Wiring
Reference setup: red 3.3 V, black GND, yellow Arduino D4. Confirm the DATA pull-up is to 3.3 V and check 5 V Arduino UNO electrical compatibility before connecting.

## API
`#include <7SemiOneWire.h>`; `SevenSemiOneWire probe(4);`; `probe.begin()`; `probe.readMeasurement(temperatureC, humidityRH)`.

## Limitations
- One sensor per library instance; this API selects the first valid ROM detected.
- The supplied reference code does not document response checksum verification; this port checks basic invalid readings but cannot authenticate every frame.
- This ZIP is source-verified against the supplied working code, but has **not** been compiled with Arduino IDE or validated on the physical probe in this environment. Test before customer release.
- `readMeasurement` reads temperature and humidity together, rather than repeating transactions three times as in the original sketch.
