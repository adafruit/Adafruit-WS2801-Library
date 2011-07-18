#include <WProgram.h>

class WS2801 {
 private:
    // the arrays of int32s that hold each LED's 24 bit color values
    uint32_t *pixels;
    uint16_t numLEDs;
    uint8_t dataPin, clockPin;
 public:
  WS2801(uint16_t n, uint8_t dpin, uint8_t cpin);
  void begin();
  void show();
  void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
  void setPixelColor(uint16_t n, uint32_t c);
  uint16_t numPixels(void);
};
