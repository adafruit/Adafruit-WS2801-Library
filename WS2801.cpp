#include "WS2801.h"

// Example to control WS2801-based RGB LED Modules in a strand or strip
// Written by Adafruit - MIT license
/*****************************************************************************/


WS2801::WS2801(uint16_t n, uint8_t dpin, uint8_t cpin) {
  dataPin = dpin;
  clockPin = cpin;
  numLEDs = n;

  pixels = (uint32_t *)malloc(numLEDs);
  for (uint16_t i=0; i< numLEDs; i++) {
    setPixelColor(i, 0, 0, 0);
  }

}

void WS2801::begin(void) {
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
}

uint16_t WS2801::numPixels(void) {
  return numLEDs;
}

void WS2801::show(void) {
    uint32_t data;
    digitalWrite(clockPin, LOW);
    delay(1);
    
    // send all the pixels
    for (uint16_t p=0; p< numLEDs; p++) {
        data = pixels[p];
	// 24 bits of data per pixel
        for (int32_t i=0x800000; i>0; i>>=1) {
            digitalWrite(clockPin, LOW);
            if (data & i)
                digitalWrite(dataPin, HIGH);
            else
                digitalWrite(dataPin, LOW);
            digitalWrite(clockPin, HIGH);    // latch on clock rise
        }
    }
    // when we're done we hold the clock pin low for a millisecond to latch it
    digitalWrite(clockPin, LOW);
    delay(1);
}

void WS2801::setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
  uint32_t data;

  if (n > numLEDs) return;

  data = g;
  data <<= 8;
  data |= b;
  data <<= 8;
  data |= r;
  
  pixels[n] = data;
}

void WS2801::setPixelColor(uint16_t n, uint32_t c) {
  if (n > numLEDs) return;

  pixels[n] = c & 0xFFFFFF;
}

