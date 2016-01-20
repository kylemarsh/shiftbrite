#include "shiftbrite.h"

#if defined(ARDUINO)
#include <SPI.h>
#endif //platform-check

/********************
 * ShiftBrite Class *
 ********************/

/*
 * Constructor:
 * ShiftBrite(num, pin)
 * Create a ShiftBrite object to drive a chain of "num" pixels using "pin" as
 * the latch pin. Allocates memory on the heap for each pixel, so if you need
 * to change the length of the chain on the fly for some reason you need to
 * destroy this object and make a new one.
 *
 * You should declare the ShiftBrite object globally at the beginning of your
 * firmware, then be sure to initialize it with MySB.begin() in your setup()
 * function.
 */
ShiftBrite::ShiftBrite(uint16_t num, uint8_t pin) :
  numLEDs(num), numBytes(num*4), latchpin(pin), pixels(NULL)
{
  // Allocate enough memory for the whole strip
  // ShiftBritePackets require 32 bits each, and
  // we have numLEDs of them.

  // FIXME: what's the best way to do this?
  if((pixels = (ShiftBritePacket *)malloc(numBytes))) {
    memset(pixels, 0, numBytes);
  }
}

ShiftBrite::~ShiftBrite()
{
  if (pixels) free(pixels);
  pinMode(latchpin, INPUT);
  SPI.end();
}

/*
 * ShiftBrite.begin()
 * Initialize the SPI connection and latch pin. This method should be called
 * from your firmware's setup() function.
 */
void ShiftBrite::begin(void)
{
  pinMode(latchpin, OUTPUT);
  digitalWrite(latchpin, LOW);

  // ShiftBrites are driven by the Allegro A6281
  // Datasheet for A6281 can be found here: https://www.pololu.com/file/download/allegroA6281.pdf?file_id=0J225
  // Particle Core's SPI library uses the following pins
  //   A2: SS (You can change this when invoking SPI.begin() but we're not using it in this library anyway)
  //   A3: SCK  - Clock
  //   A4: MISO - Master in, slave out (receiving data from a downstream device) (we don't use this)
  //   A5: MOSI - Master out, slave in (sending data to a downstream device)
  // The Allegro A6281 has a 32bit shift register and uses the following pinout:
  //   DI - Data In   - Data received from upstream device (the bit to shift in on the clock's next rising edge)
  //   LI - Latch In  - Pulse the latch high after last bit has been shifted in to "save" the shift registers
  //   EI - Enable In - Hold low to enable the LEDs. Hold high to disable the LEDs.
  //   CI - Clock In  - Clock signal from upstream.
  // Each input has a corresponding output that gets propagated through the chip to the next chip;
  // DO is the oldest bit that was shifted into the shift registries, rather than the current value of DI
  // These inputs map to the SPI outputs like so:
  //   SCK  --> CI
  //   MISO --> DI
  // We will be manually controlling LI (latch) and EI (simply connect to GND)
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
#if defined(ARDUINO)
  SPI.setClockDivider(SPI_CLOCK_DIV4); // SPI_CLOCK_DIV puts us at 4MHz on the 16MHz arduino boards.
  SPI.setDataMode(SPI_MODE0); // Arduino uses nonstandard SPI mode numbers.
#elif defined(SPARK)
  SPI.setClockSpeed(8, MHZ);  // Datasheet indicates max clock of 5 MHz, I think. Mine seem to work up to 8 MHz
  SPI.setDataMode(SPI_MODE1); // Data Mode is an implementation-level detail of SPI defining the particular timing of signals. A6281 uses mode 1
#endif //platform-check
}

void ShiftBrite::allOff(void)
{
  memset(pixels, 0, numBytes); // Cheating, I know. WORKS
  show();
}

void ShiftBrite::allOn(int16_t red, int16_t green, int16_t blue)
{
  for (uint16_t i = 0; i < numLEDs; ++i) {
    setPixelRGB(i, red, green, blue);
  }
  show();
}

// TODO: Add HSV conversion?
void ShiftBrite::setPixelRGB(uint16_t i, int16_t red, int16_t green, int16_t blue)
{
  // Limit the input to ten bits (0-1023)
  int16_t r = constrain(red,   0, 1023);
  int16_t g = constrain(green, 0, 1023);
  int16_t b = constrain(blue,  0, 1023);
#if defined(ARDUINO)
  pixels[i].red   = r;
  pixels[i].green = g;
  pixels[i].blue  = b;
#elif defined(SPARK)
  pixels[i].red   = gamma_correction[r];
  pixels[i].green = gamma_correction[g];
  pixels[i].blue  = gamma_correction[b];
#endif //platform-check
}

void ShiftBrite::setPixelRGB_no_gamma(uint16_t i, int16_t red, int16_t green, int16_t blue)
{
  pixels[i].red   = constrain(red,   0, 1023);
  pixels[i].green = constrain(green, 0, 1023);
  pixels[i].blue  = constrain(blue,  0, 1023);
}

void ShiftBrite::unsetPixel(uint16_t i)
{
  setPixelRGB(i, 0, 0, 0);
}

/*
 * ShiftBrite.show()
 * Send the packets for each pixel and then latch them in
 * Send in reverse order so pixel[0] is the "closest" shiftbrite
 * to the microcontroller.
 */
void ShiftBrite::show(void)
{
  for (uint16_t i = 1; i <= numLEDs; ++i) {
    _sendPacket(pixels[numLEDs-i]);
  }
  _latch();
}

void ShiftBrite::_sendPacket(ShiftBritePacket packet)
{
  uint32_t data = packet.value;
  SPI.transfer(data >> 24);
  SPI.transfer(data >> 16);
  SPI.transfer(data >>  8);
  SPI.transfer(data >>  0);
}

/*
 * ShiftBrite._latch() (private method)
 * Toggle the latch pin HIGH then LOW to trigger the internal latches in the
 * ShiftBrite pixels, moving the data from the shift registers into the
 * appropriate storage registers (generally the PWM registers)
 */
void ShiftBrite::_latch(void)
{
  digitalWrite(latchpin, HIGH);
  digitalWrite(latchpin, LOW);
}

