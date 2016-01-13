// This #include statement was automatically added by the Particle IDE.
#include "shiftbrite.h"

#define RegLatchPin D0 // replace with pin you use for the latch
#define numleds 4      // Number of LEDs in your chain
#define numcolors 9    // Number of colors to cycle through (adjust if you change the ColorWheel array)
#define train 1        // If 1 colors will cycle down through the chain. If 0 the whole chain will be the same color.

// FIXME: Should I put this somewhere else?
uint16_t ColorWheel[numcolors][3] = {
  {   0,    0,    0}, // Off
  {1023,    0,    0}, // Red
  {   0, 1023,    0}, // Green
  {   0,    0, 1023}, // Bue
  {   0, 1023, 1023}, // Cyan
  {1023, 1023,    0}, // Yellow
  {1023,    0, 1023}, // Magenta
  {1023, 1023, 1023}, // White
  {127,   127,  127}  // Dim White
};

ShiftBrite sb(numleds, RegLatchPin);

int pos = 0;

void setup() {
  sb.begin();
  sb.show();
}

void loop() {
  for (int i = 0; i < numleds; ++i) {
    int x = train ? i : 0;
    uint16_t *color = ColorWheel[(pos + x) % numcolors];
    sb.setPixelRGB(i, color[0], color[1], color[2]);
  }
  sb.show();

  pos = (pos + 1) % numcolors;
  delay(1000);
}
















