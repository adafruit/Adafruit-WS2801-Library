#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
 #include <pins_arduino.h>
#endif

class WS2801 {
 private:
  uint8_t *pixels; // Holds color values for each LED (3 bytes each)
  uint16_t numLEDs;
  uint8_t dataPin, clockPin;
  volatile uint8_t *clkportreg, *mosiportreg;
  uint8_t clkpin, mosipin;
  boolean hardwareSPI;
 public:
  WS2801(uint16_t n, uint8_t dpin, uint8_t cpin); // Configurable pins
  WS2801(uint16_t n); // Use SPI hardware; specific pins only
  void begin();
  void show();
  void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
  void setPixelColor(uint16_t n, uint32_t c);
  uint16_t numPixels(void);
};
