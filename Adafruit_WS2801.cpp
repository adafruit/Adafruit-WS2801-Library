#include "Adafruit_WS2801.h"

// Example to control WS2801-based RGB LED Modules in a strand or strip
// Written by Adafruit - MIT license
/*****************************************************************************/

#ifdef __AVR_ATtiny85__

// Teensy/Gemma-specific stuff for hardware-assisted SPI @ 1 MHz

#if(F_CPU > 8000000L)
  #define SPI_DELAY asm volatile("rjmp .+0\nrjmp .+0"); // Burn 4 cycles
#elif(F_CPU > 4000000L)
  #define SPI_DELAY asm volatile("rjmp .+0");           // Burn 2 cycles
#else
  #define SPI_DELAY                                     // Run max speed
#endif

#define SPIBIT                                  \
  USICR = ((1<<USIWM0)|(1<<USITC));             \
  SPI_DELAY                                     \
  USICR = ((1<<USIWM0)|(1<<USITC)|(1<<USICLK)); \
  SPI_DELAY

static void spi_out(uint8_t n) {
  USIDR = n;
  SPIBIT
  SPIBIT
  SPIBIT
  SPIBIT
  SPIBIT
  SPIBIT
  SPIBIT
  SPIBIT
}

#else

// All other boards support Full and Proper Hardware SPI

#include <SPI.h>
#define spi_out(n) (void)SPI.transfer(n)

#endif

/*****************************************************************************/

// Constructor for use with hardware SPI (specific clock/data pins):
Adafruit_WS2801::Adafruit_WS2801(uint16_t n, uint8_t order) {
  rgb_order = order;
  alloc(n);
  updatePins();
}

// Constructor for use with arbitrary clock/data pins:
Adafruit_WS2801::Adafruit_WS2801(uint16_t n, uint8_t dpin, uint8_t cpin, uint8_t order) {
  rgb_order = order;
  alloc(n);
  updatePins(dpin, cpin);
}

// Constructor for use with a matrix configuration, specify w, h for size
// of matrix.  Assumes configuration where string starts at coordinate 0,0
// and continues to w-1,0, w-1,1 and on to 0,1, 0,2 and on to w-1,2 and
// so on. Snaking back and forth till the end.  Other function calls
// provide access to pixels via an x,y coordinate system
Adafruit_WS2801::Adafruit_WS2801(uint16_t w, uint16_t h, uint8_t dpin, uint8_t cpin, uint8_t order) {
  rgb_order = order;
  alloc(w * h);
  width  = w;
  height = h;
  updatePins(dpin, cpin);
}

// Allocate 3 bytes per pixel, init to RGB 'off' state:
void Adafruit_WS2801::alloc(uint16_t n) {
  begun   = false;
  numLEDs = ((pixels = (uint8_t *)calloc(n, 3)) != NULL) ? n : 0;
}

// via Michael Vogt/neophob: empty constructor is used when strand length
// isn't known at compile-time; situations where program config might be
// read from internal flash memory or an SD card, or arrive via serial
// command.  If using this constructor, MUST follow up with updateLength()
// and updatePins() to establish the strand length and output pins!
// Also, updateOrder() to change RGB vs GRB order (RGB is default).
Adafruit_WS2801::Adafruit_WS2801(void) {
  begun     = false;
  numLEDs   = 0;
  pixels    = NULL;
  rgb_order = WS2801_RGB;
  updatePins(); // Must assume hardware SPI until pins are set
}

// Release memory (as needed):
Adafruit_WS2801::~Adafruit_WS2801(void) {
  if(pixels) free(pixels);
}

// Activate hard/soft SPI as appropriate:
void Adafruit_WS2801::begin(void) {
  if(hardwareSPI == true) {
    startSPI();
  } else {
    pinMode(datapin, OUTPUT);
    pinMode(clkpin , OUTPUT);
  }
  begun = true;
}

// Change pin assignments post-constructor, switching to hardware SPI:
void Adafruit_WS2801::updatePins(void) {
  pinMode(datapin, INPUT); // Restore data and clock pins to inputs
  pinMode(clkpin , INPUT);
  datapin     = clkpin = 0;
  hardwareSPI = true;
  // If begin() was previously invoked, init the SPI hardware now:
  if(begun == true) startSPI();
  // Otherwise, SPI is NOT initted until begin() is explicitly called.
}

// Change pin assignments post-constructor, using arbitrary pins:
void Adafruit_WS2801::updatePins(uint8_t dpin, uint8_t cpin) {
  if(begun == true) { // If begin() was previously invoked...
    // If previously using hardware SPI, turn that off:
    if(hardwareSPI) {
#ifdef __AVR_ATtiny85__
      DDRB &= ~(_BV(PORTB1) | _BV(PORTB2));
#else
      SPI.end();
#endif
    } else {
      pinMode(datapin, INPUT); // Restore prior data and clock pins to inputs
      pinMode(clkpin , INPUT);
    }
    pinMode(dpin, OUTPUT); // Enable output on 'soft' SPI pins:
    pinMode(cpin, OUTPUT);
  }

  datapin     = dpin;
  clkpin      = cpin;
#ifdef __AVR__
  clkport     = portOutputRegister(digitalPinToPort(cpin));
  clkpinmask  = digitalPinToBitMask(cpin);
  dataport    = portOutputRegister(digitalPinToPort(dpin));
  datapinmask = digitalPinToBitMask(dpin);
#endif
  hardwareSPI = false;
}

// Enable SPI hardware and set up protocol details:
void Adafruit_WS2801::startSPI(void) {
#ifdef __AVR_ATtiny85__
  PORTB &= ~(_BV(PORTB1) | _BV(PORTB2)); // Outputs
  DDRB  |=   _BV(PORTB1) | _BV(PORTB2);  // DO (NOT MOSI) + SCK
#else
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
 #if defined(__AVR__) || defined(CORE_TEENSY)
  SPI.setClockDivider(SPI_CLOCK_DIV16); // 1MHz max, else flicker
 #else
  SPI.setClockDivider((F_CPU + 500000L) / 1000000L);
 #endif
#endif
}

uint16_t Adafruit_WS2801::numPixels(void) {
  return numLEDs;
}

// Change strand length (see notes with empty constructor, above):
void Adafruit_WS2801::updateLength(uint16_t n) {
  if(pixels != NULL) free(pixels); // Free existing data (if any)
  // Allocate new data -- note: ALL PIXELS ARE CLEARED
  numLEDs = ((pixels = (uint8_t *)calloc(n, 3)) != NULL) ? n : 0;
  // 'begun' state does not change -- pins retain prior modes
}

// Change RGB data order (see notes with empty constructor, above):
void Adafruit_WS2801::updateOrder(uint8_t order) {
  rgb_order = order;
  // Existing LED data, if any, is NOT reformatted to new data order.
  // Calling function should clear or fill pixel data anew.
}

void Adafruit_WS2801::show(void) {
  uint16_t i, nl3 = numLEDs * 3; // 3 bytes per LED
  uint8_t  bit;

  // Write 24 bits per pixel:
  if(hardwareSPI) {
    for(i=0; i<nl3; i++) spi_out(pixels[i]);
  } else {
    for(i=0; i<nl3; i++ ) {
      for(bit=0x80; bit; bit >>= 1) {
#ifdef __AVR__
        if(pixels[i] & bit) *dataport |=  datapinmask;
        else                *dataport &= ~datapinmask;
        *clkport |=  clkpinmask;
        *clkport &= ~clkpinmask;
#else
        if(pixels[i] & bit) digitalWrite(datapin, HIGH);
        else                digitalWrite(datapin, LOW);
        digitalWrite(clkpin, HIGH);
        digitalWrite(clkpin, LOW);
#endif
      }
    }
  }

  delay(1); // Data is latched by holding clock pin low for 1 millisecond
}

// Set pixel color from separate 8-bit R, G, B components:
void Adafruit_WS2801::setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
  if(n < numLEDs) { // Arrays are 0-indexed, thus NOT '<='
    uint8_t *p = &pixels[n * 3];
    // See notes later regarding color order
    if(rgb_order == WS2801_RGB) {
      *p++ = r;
      *p++ = g;
    } else {
      *p++ = g;
      *p++ = r;
    }
    *p++ = b;
  }
}

// Set pixel color from separate 8-bit R, G, B components using x,y coordinate system:
void Adafruit_WS2801::setPixelColor(uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b) {
  boolean evenRow = ((y % 2) == 0);
  // calculate x offset first
  uint16_t offset = x % width;
  if (!evenRow) {
    offset = (width-1) - offset;
  }
  // add y offset
  offset += y * width;
  setPixelColor(offset, r, g, b);
}

// Set pixel color from 'packed' 32-bit RGB value:
void Adafruit_WS2801::setPixelColor(uint16_t n, uint32_t c) {
  if(n < numLEDs) { // Arrays are 0-indexed, thus NOT '<='
    uint8_t *p = &pixels[n * 3];
    // To keep the show() loop as simple & fast as possible, the
    // internal color representation is native to different pixel
    // types.  For compatibility with existing code, 'packed' RGB
    // values passed in or out are always 0xRRGGBB order.
    if(rgb_order == WS2801_RGB) {
      *p++ = c >> 16; // Red
      *p++ = c >>  8; // Green
    } else {
      *p++ = c >>  8; // Green
      *p++ = c >> 16; // Red
    }
    *p++ = c;         // Blue
  }
}

// Set pixel color from 'packed' 32-bit RGB value using x,y coordinate system:
void Adafruit_WS2801::setPixelColor(uint16_t x, uint16_t y, uint32_t c) {
  boolean evenRow = ((y % 2) == 0);
  // calculate x offset first
  uint16_t offset = x % width;
  if (!evenRow) {
    offset = (width-1) - offset;
  }
  // add y offset
  offset += y * width;
  setPixelColor(offset, c);
}

// Query color from previously-set pixel (returns packed 32-bit RGB value)
uint32_t Adafruit_WS2801::getPixelColor(uint16_t n) {
  if(n < numLEDs) {
    uint16_t ofs = n * 3;
    // To keep the show() loop as simple & fast as possible, the
    // internal color representation is native to different pixel
    // types.  For compatibility with existing code, 'packed' RGB
    // values passed in or out are always 0xRRGGBB order.
    return (rgb_order == WS2801_RGB) ?
      ((uint32_t)pixels[ofs] << 16) | ((uint16_t) pixels[ofs + 1] <<  8) | pixels[ofs + 2] :
      (pixels[ofs] <<  8) | ((uint32_t)pixels[ofs + 1] << 16) | pixels[ofs + 2];
  }

  return 0; // Pixel # is out of bounds
}

