#include "7SemiOneWire.h"
#include <string.h>

// Exact application command bytes from the supplied working temp_humidity.h.
static const uint8_t kCommand1[] = {0xCC, 0x66, 0x05, 0x83, 0x0B, 0x03, 0xA5, 0x0F, 0x75, 0x02, 0xAA};
static const uint8_t kCommand2[] = {0x55, 0x56, 0xBC, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x66, 0x05, 0x83, 0x0B, 0x03, 0xA5, 0x0F, 0x75, 0x02, 0xAA};
static const uint8_t kCommand3[] = {0x55, 0x56, 0xBC, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x66, 0x01, 0x7A, 0x9F, 0x93, 0xAA};
static const uint8_t kCommand4[] = {0x55, 0x56, 0xBC, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x66, 0x01, 0x6A, 0x9E, 0x5F, 0xAA};
static const uint8_t kCommand5[] = {0x55, 0x56, 0xBC, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x66, 0x02, 0x55, 0x01, 0xFE, 0x26, 0xAA};
static const uint8_t kCommand6[] = {0x55, 0x56, 0xBC, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x66, 0x18, 0x11, 0x00, 0x00, 0x02, 0xE3, 0x02, 0x88, 0xFD, 0x03, 0xDD, 0x08, 0x02, 0xE3, 0x01, 0x89, 0xD4, 0x06, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03, 0xC8, 0x30, 0xAA};
static const uint8_t kCommand7[] = {0x55, 0x56, 0xBC, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x66, 0x03, 0x22, 0x00, 0x2A, 0xD6, 0x69, 0xAA};
static const uint8_t kCommand8[] = {0x55, 0x56, 0xBC, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x66, 0x04, 0x33, 0x00, 0x2A, 0x00, 0x18, 0xDD, 0xAA};
static const uint8_t kCommand9[] = {0x55, 0x56, 0xBC, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x66, 0x03, 0x22, 0x0E, 0x0C, 0x5D, 0xBC, 0xAA};

SevenSemiOneWire::SevenSemiOneWire(uint8_t pin) : bus_(pin), count_(0), initialized_(false) { memset(rom_, 0, sizeof(rom_)); }
float SevenSemiOneWire::toCelsius(uint16_t raw) { return -45.0f + 175.0f * (float)raw / 65535.0f; }
float SevenSemiOneWire::toHumidity(uint16_t raw) { return -6.0f + 125.0f * (float)raw / 65535.0f; }
bool SevenSemiOneWire::readROM(uint8_t address[8]) const { if (!count_) return false; memcpy(address, rom_, 8); return true; }
bool SevenSemiOneWire::writeFrame(uint8_t number) {
  const uint8_t *frame = NULL;
  uint8_t length = 0;
  switch (number) {
    case 1: frame = kCommand1; length = sizeof(kCommand1); break;
    case 2: frame = kCommand2; length = sizeof(kCommand2); break;
    case 3: frame = kCommand3; length = sizeof(kCommand3); break;
    case 4: frame = kCommand4; length = sizeof(kCommand4); break;
    case 5: frame = kCommand5; length = sizeof(kCommand5); break;
    case 6: frame = kCommand6; length = sizeof(kCommand6); break;
    case 7: frame = kCommand7; length = sizeof(kCommand7); break;
    case 8: frame = kCommand8; length = sizeof(kCommand8); break;
    case 9: frame = kCommand9; length = sizeof(kCommand9); break;
    default: return false;
  }
  if (!bus_.reset()) return false;
  // Preserve original working sketch's power=1 argument for every byte.
  // The bus is reset at the start of each frame, as in the reference.
  for (uint8_t i = 0; i < length; ++i) {
    uint8_t value = frame[i];
    if (number != 1 && i >= 1 && i <= 8) value = rom_[i - 1];
    bus_.write(value, 1);
  }
  delay(80);
  return true;
}
void SevenSemiOneWire::readFrame(uint8_t *response, uint8_t length) {
  for (uint8_t i = 0; i < length; ++i) {
    uint8_t b = bus_.read();
    if (response) response[i] = b;
  }
  delay(200);
}
bool SevenSemiOneWire::runFrame(uint8_t number, uint8_t *response, uint8_t length) {
  if (!writeFrame(number)) return false;
  readFrame(response, length);
  return true;
}
bool SevenSemiOneWire::begin() {
  initialized_ = false;
  count_ = 0;
  bus_.reset_search();
  uint8_t found[8];
  // Original reference supports multiple devices; this customer API targets one.
  for (uint8_t attempt = 0; attempt < 5; ++attempt) {
    bus_.reset_search();
    if (bus_.search(found) && OneWire::crc8(found, 7) == found[7]) {
      memcpy(rom_, found, 8);
      count_ = 1;
      break;
    }
    delay(100);
  }
  if (!count_) return false;
  // Exact initialization order from working temp_humidity.h:
  // 1 -> 2 -> 3 -> 5 -> 4 -> 6 -> 7.
  if (!writeFrame(1)) return false;
  delay(900);
  if (!runFrame(2, NULL, 5)) return false;
  if (!runFrame(3, NULL, 9)) return false;
  if (!runFrame(5, NULL, 5)) return false;
  if (!runFrame(4, NULL, 6)) return false;
  if (!runFrame(6, NULL, 5)) return false;
  if (!runFrame(7, NULL, 24)) return false;
  initialized_ = true;
  return true;
}
bool SevenSemiOneWire::readMeasurement(float &temperatureC, float &humidityRH) {
  if (!initialized_) return false;
  uint8_t response[10];
  // Each measurement requires command 8 then command 9.
  if (!runFrame(8, NULL, 7)) return false;
  if (!runFrame(9, response, sizeof(response))) return false;
  const uint16_t rawT = ((uint16_t)response[3] << 8) | response[4];
  const uint16_t rawH = ((uint16_t)response[6] << 8) | response[7];
  // Defensive checks; original reference has no documented response CRC.
  bool allFF = true, all00 = true;
  for (uint8_t i = 0; i < sizeof(response); ++i) {
    if (response[i] != 0xFF) allFF = false;
    if (response[i] != 0x00) all00 = false;
  }
  if (allFF || all00 || rawT == 0xFFFF || rawH == 0xFFFF) return false;
  float t = toCelsius(rawT), h = toHumidity(rawH);
  if (t < -45.0f || t > 130.0f || h < 0.0f || h > 100.0f) return false;
  temperatureC = t;
  humidityRH = h;
  return true;
}
