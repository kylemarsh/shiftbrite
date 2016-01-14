// This #include statement was automatically added by the Particle IDE.
#include "particle_shfitbrite/shiftbrite.h"

#define RegLatchPin D0 // replace with pin you use for the latch
#define numleds 4      // Number of LEDs in your chain
#define numcolors 9    // Number of colors to cycle through (adjust if you change the ColorWheel array)

#define ButtonPin D5

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

int mode = 0;
int pos = 0;

void setup() {
  // Set an interrupt on a button to cycle through the example modes.
  // This may or may not actually work yet.
  pinMode(ButtonPin, INPUT_PULLUP);
  attachInterrupt(ButtonPin, next_mode, FALLING);

  sb.begin();
  sb.show();
}

void loop() {
  switch(mode) {
    case 0:
      train_test(0);
      break;
    case 1:
      train_test(1);
      break;
    case 2:
      gamma_test1();
      break;
    case 3:
      gamma_test2();
      break;
    default:
      train_test(1);
      break;
  }
}

void gamma_test1() {
  // Illustrate the difference between a pixel at 50% brightness
  // with gamma currection and without gamma correction
  // Assumes 4 pixels
  sb.setPixelRGB(0, 1023, 1023, 1023);
  sb.setPixelRGB(1,  511,  511,  511);
  sb.setPixelRGB_no_gamma(2,  511,  511,  511);
  sb.setPixelRGB_no_gamma(3, 1023, 1023, 1023);
  sb.show();
  delay(500);
}

void gamma_test2() {
  // Set each pixel to half the brightness of its predecessor
  // Assumes 4 pixels
  sb.setPixelRGB(0, 1023, 1023, 1023);
  sb.setPixelRGB(1,  511,  511,  511);
  sb.setPixelRGB(2,  255,  255,  255);
  sb.setPixelRGB(3,  127,  127,  127);
  sb.show();
  delay(500);
}

void train_test(bool train) {
  // cycle through the colorwheel defined at the top of this file
  // if "train" is true, each subsequent pixel is a step ahead of the previous
  // otherwise all pixels are the same color
  for (int i = 0; i < numleds; ++i) {
    int x = train ? i : 0;
    uint16_t *color = ColorWheel[(pos + x) % numcolors];
    sb.setPixelRGB(i, color[0], color[1], color[2]);
  }
  sb.show();
  pos = (pos + 1) % numcolors;
  delay(500);
}

void next_mode(void) {
  mode = (mode + 1) % 4;
}
