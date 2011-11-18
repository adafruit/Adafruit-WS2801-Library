#include "SPI.h"
#include "WS2801.h"
#include "SPI.h"
#include "wiring_private.h"

// Example to control WS2801-based RGB LED Modules in a strand or strip
// Written by Adafruit - MIT license
/*****************************************************************************/

WS2801::WS2801(uint16_t n) {
  // Use hardware SPI
  dataPin = clockPin = 0;
  hardwareSPI = true;

  numLEDs = n;

  // malloc 3 bytes per pixel so we dont have to hardcode the length
  pixels = (uint8_t *)malloc(numLEDs * 3); // 3 bytes per pixel
  memset(pixels, 0, numLEDs * 3); // Init to RGB 'off' state
}

WS2801::WS2801(uint16_t n, uint8_t dpin, uint8_t cpin) {
  dataPin = dpin;
  clockPin = cpin;
  numLEDs = n;

  clkportreg = portOutputRegister(digitalPinToPort(cpin));
  clkpin = digitalPinToBitMask(cpin);
  mosiportreg = portOutputRegister(digitalPinToPort(dpin));
  mosipin = digitalPinToBitMask(dpin);

  // malloc 3 bytes per pixel so we dont have to hardcode the length
  pixels = (uint8_t *)malloc(numLEDs * 3); // 3 bytes per pixel
  memset(pixels, 0, numLEDs * 3); // Init to RGB 'off' state
}

void WS2801::begin(void) {
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  if (hardwareSPI) {
    // using hardware SPI
    SPI.begin();
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
    SPI.setClockDivider(SPI_CLOCK_DIV8);  // 2 MHz
    // WS2801 datasheet recommends max SPI clock of 2 MHz, and 50 Ohm
    // resistors on SPI lines for impedance matching.  In practice and
    // at short distances, 2 MHz seemed to work reliably enough without
    // resistors, and 4 MHz was possible with a 220 Ohm resistor on the
    // SPI clock line only.  Your mileage may vary.  Experiment!
    // SPI.setClockDivider(SPI_CLOCK_DIV4);  // 4 MHz
  }
}

uint16_t WS2801::numPixels(void) {
  return numLEDs;
}

void WS2801::show(void) {
  uint16_t i, nl3 = numLEDs * 3; // 3 bytes per LED
  uint8_t bit;

  // write 24 bits per pixel
  if (hardwareSPI) {
    for (i=0; i<nl3; i++ ) {
      SPDR = pixels[i];
      while (!(SPSR & (1<<SPIF))) {};
    }
  } else {
    for (i=0; i<nl3; i++ ) {
      for (bit=0x80; bit; bit >>= 1) {
        *clkportreg &= ~clkpin;
        if (pixels[i] & bit) {
          *mosiportreg |= mosipin;
        } else {
          *mosiportreg &= ~mosipin;
        }
        *clkportreg |= clkpin;
      }
    }
    *clkportreg &= ~clkpin;
  }

  // when we're done we hold the clock pin low for a millisecond to latch it
  delay(1);
}

void WS2801::setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
  if (n >= numLEDs) return; // '>=' because arrays are 0-indexed

  n *= 3;
  pixels[n++] = r;
  pixels[n++] = g;
  pixels[n  ] = b;
}

void WS2801::setPixelColor(uint16_t n, uint32_t c) {
  if (n >= numLEDs) return; // '>=' because arrays are 0-indexed

  n *= 3;
  pixels[n++] = c >> 16;
  pixels[n++] = c >> 8;
  pixels[n  ] = c;

}

