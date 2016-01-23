# shiftbrite
Library for driving a chain of ShiftBrite LED pixels from Particle and Arduino
boards

## Basic Usage

To get started using the shiftbrite driver, import the library and intantiate
an object:

```cpp
#include "shiftbrite.h"
#define NUM_PIXELS 3
#define LATCH_PIN D0

ShiftBrite sb = ShiftBrite(NUM_PIXELS, LATCH_PIN);
```

The ShiftBrite constructor takes 2 arguments:
 - the number of ShiftBrite pixels in this chain
 - which pin to use as the "latch" pin. This can be any available pin.

Once you've constructed the object at the global scope you need to initialize
the hardware in your `setup()` function:

```cpp
void setup() {
	sb.begin();
	sb.show();
}
```

You should probably also call `show()` on your object to avoid any noise from
the hardware power up from showing weird colors.

Finally assign a color to a pixel!

```cpp
void loop() {
	for (int i = 0; i < NUM_PIXELS; ++i) {
		sb.setPixelRGB(i, 1023, 511, 255);
	}
	sb.show();
}
```

Colors are passed as three 16-bit integers<sup>*</sup>


<sup>*</sup>The ShiftBrite's driver board has a 10-bit PWM register for each
color channel, so you should only pass values from 0 to 1023. The data type
for the color values is int16_t so you can use negative numbers in your math.

## Available Interface

The public methods on the class are as follows:

### Constructor `ShiftBrite(uint16_t num, uint8_t pin)`

Builds a new object representing a chain of ShiftBrite pixels
 - `num`: the number of ShiftBrite pixels in this chain
 - `pin`: the pin to use for latching data into the PWM registers

### `begin()`

Initializes the SPI hardware and latch pin. You must call this before the
ShiftBrite chain will function (probably in your firmware's `setup()`
function). This function configures the SPI port with specific settings and
doesn't currently clean up between transactions, so you may not be able to use
other SPI devices with this library if they require different clock speeds, SPI
modes, or byte orders.

### `show()`

Write the current color data to the ShiftBrites over the SPI bus. Loops over
the currently stored data for each pixel in the chain and sends that data to
the pixels. Finishes by latching the data into the PWM registers, causing the
LEDs to reflect the change.

Pixel data is sent such that the pixel at address 0 is the closest to the
microcontroller.

### `allOff()`

Write zeroes to every color channel on every pixel in the chain. This turns
every pixel off entirely at the same time. Calls `show()` for you.

### `allOn(int16_t red, int16_t green, int16_t blue)`

Write the given color to every pixel in the chain. Colors are described by
three 10-bit values: red, green, and blue. Values outside of the range from 0
to 1023 are constrained to that range. Calls `show()` for you.

### `setPixelRGB(uint16_t i, int16_t red, int16_t green, int16_t blue)`

Write the given color to the specified pixel (addressed from 0 to
`num_pixels - 1` where 0 is the closest pixel to the microcontroller). Colors
are described by three 10-bit values: red, green, and blue. Values outside of
the range from 0 to 1023 are constrained to that range. Passes each value
through a gamma-correcting function<sup>*</sup> so that perceived brightness is
approximately linear as the values change from 0 to 1023.

`setPixelRGB(...)` does not call `show()` for you.

<sup>*</sup>Gamma correction currently only available on Particle boards; I
haven't got it working quite right on Arduino yet

### `setPixelRGB_no_gamma(uint16_t i, int16_t red, int16_t green, int16_t blue)`

As `setPixelRGB(...)` but does not pass values through gamma-correction
function first. This means the raw PWM output for each channel will be exactly
what you set, but since the human eye's response to light is non-linear a value
of 511 will appear far brighter than half as bright as a value of 1023.

`setPixelRGB_no_gamma(...)` does not call `show()` for you.

### `unsetPixel(uint16_t i)`

Turn off the pixel at address `i` (addresses begin with the closest pixel to the
microcontroller at 0 and increment until `num_pixels -1`).

