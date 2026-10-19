#include <7SemiOneWire.h>

/* 7Semi One Wire Temperature and Humidity Probe
   Arduino UNO: RED=3.3V, BLACK=GND, YELLOW=D4.
   Verify 3.3V DATA pull-up and UNO voltage compatibility.
   Requires the Arduino OneWire library.
*/
SevenSemiOneWire probe(4);

void setup() {
  Serial.begin(9600);
  Serial.println(F("7Semi One Wire Temperature/Humidity"));
  if (!probe.begin()) {
    Serial.println(F("Sensor initialization failed"));
  } else {
    Serial.println(F("Sensor initialized"));
  }
}

void loop() {
  float temperatureC = 0.0f;
  float humidityRH = 0.0f;
  if (probe.readMeasurement(temperatureC, humidityRH)) {
    Serial.print(F("Temperature: "));
    Serial.print(temperatureC, 2);
    Serial.print(F(" C | Humidity: "));
    Serial.print(humidityRH, 2);
    Serial.println(F(" %RH"));
  } else {
    Serial.println(F("Read failed: check sensor connection and initialization"));
  }
  delay(1000);
}
