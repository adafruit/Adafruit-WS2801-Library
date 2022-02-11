#include <ArduinoUnitTests.h>
#include "../Adafruit_WS2801.h"

unittest(set_strip_values)
{
  GodmodeState* state = GODMODE();
  state->reset();

  // create strip of length 10
  Adafruit_WS2801 strip;
  strip = Adafruit_WS2801(10, 3, 2);
  strip.begin();
  strip.show();
  assertEqual(30, state->digitalPin[3].toAscii(1, true).length());

  // set all 10 pixels to some arbitrary HSV value
  for (int i = 0; i < 10; ++i) {
    strip.setPixelColor(i, 500);
  }
  strip.show();

  assertEqual(60, state->digitalPin[3].toAscii(1, true).length());

  // pin 2 is the clock pin, we expect 0xAA, which is 0b10101010
  assertEqual(0xAA, (unsigned char)(state->digitalPin[2].toAscii(1, true)[0]));
  assertEqual(0xAA, (unsigned char)(state->digitalPin[2].toAscii(1, true)[1]));
  assertEqual(0xAA, (unsigned char)(state->digitalPin[2].toAscii(1, true)[2]));

  // pin 3 is the data, we expect 3 byte groups, all zeroes
  assertEqual(0, (unsigned int)(state->digitalPin[3].toAscii(1, true)[0]));
  assertEqual(0, (unsigned int)(state->digitalPin[3].toAscii(1, true)[1]));
  assertEqual(0, (unsigned int)(state->digitalPin[3].toAscii(1, true)[2]));

  // the second 30 bytes will be our pixel color.  not sure what to expect
  // but I _do_ expect it to repeat
  assertEqual(0, (unsigned int)(state->digitalPin[3].toAscii(1, true)[30]));
  assertEqual(1, (unsigned int)(state->digitalPin[3].toAscii(1, true)[31]));
  assertEqual(4294967284, (unsigned int)(state->digitalPin[3].toAscii(1, true)[32]));
  assertEqual(0, (unsigned int)(state->digitalPin[3].toAscii(1, true)[33]));
  assertEqual(1, (unsigned int)(state->digitalPin[3].toAscii(1, true)[34]));
  assertEqual(4294967284, (unsigned int)(state->digitalPin[3].toAscii(1, true)[35]));
}

unittest_main()
