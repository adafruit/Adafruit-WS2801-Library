/*!
 * @file Adafruit_WS2801.h
 */

#ifndef __ADAFRUIT_WS2801__
#define __ADAFRUIT_WS2801__

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#include <pins_arduino.h>
#endif

// Not all LED pixels are RGB order; 36mm type expects GRB data.
// Optional flag to constructors indicates data order (default if
// unspecified is RGB).  As long as setPixelColor/getPixelColor are
// used, other code can always treat 'packed' colors as RGB; the
// library will handle any required translation internally.
#define WS2801_RGB 0 //!< Sets mode to RGB
#define WS2801_GRB 1 //!< Sets mode to GRB

/*!
 * @brief Class that stores state and functions for interacting with WS2801 LEDs
 */
class Adafruit_WS2801 {

public:
  /*!
   * @brief Constructor for use with arbitrary clock/data pins
   * @param n How many LEDs there are
   * @param dpin Data pin
   * @param cpin Clock pin
   * @param order RGB or GRB
   */
  Adafruit_WS2801(uint16_t n, uint8_t dpin, uint8_t cpin,
                  uint8_t order = WS2801_RGB);
  /*!
   * @brief Constructor for use with a matrix configuration, specify w, h for
   * size of matrix.
   * @param x Width of the matrix
   * @param y Height of the matrix
   * @param dpin Data pin
   * @param cpin Clock pin
   * @param order RGB or GRB
   */
  Adafruit_WS2801(uint16_t x, uint16_t y, uint8_t dpin, uint8_t cpin,
                  uint8_t order = WS2801_RGB);
  /*!
   * @brief Constructor for use with hardware SPI (specific clock/data pins)
   * @param n
   * @param order
   */
  Adafruit_WS2801(uint16_t n, uint8_t order = WS2801_RGB);
  /*!
   * @brief Empty constructor; init pins/strand length/data order later:
   */
  Adafruit_WS2801();
  /*!
   * @brief Release memory (as needed):
   */
  ~Adafruit_WS2801();

  void
    /*!
     * @brief Activate SPI
     */
    begin(void),
    /*!
     * @brief Shows the pixels
     */
    show(void),
    /*!
     * @brief Set pixel color from separate 8-bit R, G, B components
     * @param n Pixel to set
     * @param r Red value
     * @param g Green value
     * @param b Blue value
     */
    setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b),
    /*!
     * @brief Set pixel color from 'packed' 32-bit RGB value
     * @param n Pixel to set
     * @param c packed 32-bit RGB value to set the pixel
     */
    setPixelColor(uint16_t n, uint32_t c),
    /*!
     * @brief Set pixel color from separate 8-bit R, G, B components using x,y coordinate system
     * @param x Pixel x
     * @param y Pixel y
     * @param r Red value
     * @param g Green value
     * @param b Blue value
     */
    setPixelColor(uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b),
    /*!
     * @brief Set pixel color from 'packed' 32-bit RGB value using x,y coordinate system:
     * @param x Pixel x
     * @param y Pixel y
     * @param c packed 32-bit RGB value to set the pixel
     */
    setPixelColor(uint16_t x, uint16_t y, uint32_t c),
    /*!
     * @brief Clear the entire string
     */
    clear(),
    /*!
     * @brief Change pin assignment post-constructor, using arbitrary pins
     * @param dpin Data pin
     * @param cpin Clock pin
     */
    updatePins(uint8_t dpin, uint8_t cpin),
    /*!
     * @brief Change pin assignment post-constructor, switching to hardware SPI 
     */
    updatePins(void),
    /*!
     * @brief Change strand length
     * @param n Length to change to
     */
    updateLength(uint16_t n),
    /*!
     * @brief Change RGB data order
     * @param order Order to switch to
     */
    updateOrder(uint8_t order);
  uint16_t
    /*!
     * @brief Returns the number of pixels currently connected
     * @return Returns the number of connected pixels
     */
    numPixels(void);
  uint32_t
    /*!
     * @brief Query color from previously-set pixel
     * @param n Pixel to query
     * @return Returns packed 32-bit RGB value
     */
    getPixelColor(uint16_t n);

private:
  uint16_t numLEDs,
      width,       // used with matrix mode
      height;      // used with matrix mode
  uint8_t *pixels, // Holds color values for each LED (3 bytes each)
      rgb_order,   // Color order; RGB vs GRB (or others, if needed in future)
      clkpin, datapin; // Clock & data pin numbers
#ifdef __AVR__
  uint8_t clkpinmask, datapinmask;      // Clock & data PORT bitmasks
  volatile uint8_t *clkport, *dataport; // Clock & data PORT registers
#endif
  void alloc(uint16_t n), startSPI(void);
  boolean hardwareSPI, // If 'true', using hardware SPI
      begun;           // If 'true', begin() method was previously invoked
};

#endif // __ADAFRUIT_WS2801__
