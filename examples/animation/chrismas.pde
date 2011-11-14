#include "WS2801.h"

/*****************************************************************************
Example sketch for driving WS2801 pixels
*****************************************************************************/

// Choose which 2 pins you will use for output.
// Can be any valid output pins.
// The colors of the wires may be totally different so
// BE SURE TO CHECK YOUR PIXELS TO SEE WHICH WIRES TO USE!
int dataPin = 2;       
int clockPin = 3;      
// Don't forget to connect the ground wire to Arduino ground, and the +5V wire to a +5V supply


struct anim {
  byte ofs;
  byte pos;
  byte del;
  byte length;
  uint32_t col;
};

anim mover;
uint32_t clearCol;

// Set the first variable to the NUMBER of pixels. 25 = 25 pixels in a row
WS2801 strip = WS2801(64, dataPin, clockPin);

void setup() {
  strip.begin();
  
  newAnimation();
  
  // Update the strip, to start they are all 'off'
  strip.show();
}


void loop() {
  if (mover.pos > 0 && mover.pos == mover.del) {
    newAnimation();
  }
  
  if (mover.pos > mover.length) {
    mover.del++;
  } else {
    mover.pos++;      
  }
    
  for (int i=0; i < strip.numPixels(); i++) {
    if (i>=mover.ofs+mover.del && i<mover.ofs+mover.pos) {
      strip.setPixelColor(i, mover.col);
    } else {
      strip.setPixelColor(i, clearCol);
    }      
  }
  
  delay(20);
  strip.show(); 
}

void newAnimation() {
  mover.ofs = random(strip.numPixels()); 
  mover.length = random( strip.numPixels()-mover.ofs ); 
  mover.pos = 0;
  mover.del = 0;
  mover.col = Color(random(150), random(150), random(250));
  
  
  clearCol = Color(random(50), random(50), random(50));
}

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b) {
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}



