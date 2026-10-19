#ifndef SEVENSEMI_ONEWIRE_H
#define SEVENSEMI_ONEWIRE_H
#include <Arduino.h>
#include <OneWire.h>
class SevenSemiOneWire {
public:
  explicit SevenSemiOneWire(uint8_t pin);
  bool begin();
  bool readMeasurement(float &temperatureC, float &humidityRH);
  bool readROM(uint8_t address[8]) const;
  uint8_t deviceCount() const { return count_; }
  static float toCelsius(uint16_t raw);
  static float toHumidity(uint16_t raw);
private:
  OneWire bus_;
  uint8_t rom_[8];
  uint8_t count_;
  bool initialized_;
  bool writeFrame(uint8_t number);
  void readFrame(uint8_t *response, uint8_t length);
  bool runFrame(uint8_t number, uint8_t *response, uint8_t length);
};
#endif
