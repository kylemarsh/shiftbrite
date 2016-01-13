/*
 Particle Core library for controlling ShiftBrite LED pixels.

 License: CC BY-SA 3.0: Creative Commons Share-alike 3.0. Feel free
 to use and abuse this code however you'd like. If you find it useful
 please attribute, and SHARE-ALIKE!

 Created January 2016
 by Kyle Marsh
*/
#include "application.h"

#ifndef ShiftBrite_h
#define ShiftBrite_h

class ShiftBrite {
  public:
    ShiftBrite(uint16_t num, uint8_t pin);
    ~ShiftBrite();

    void
      begin(void),
      show(void),
      allOff(void),
      allOn(uint16_t red, uint16_t green, uint16_t blue),
      setPixelRGB(uint16_t i, uint16_t red, uint16_t green, uint16_t blue),
      unsetPixel(uint16_t i);

  private:
    typedef union
    {
      uint32_t value;
      struct // Current control and clock mode registers
      {
        unsigned greenDotCorrect:7;
        unsigned clockMode:2;
        unsigned :1;
        unsigned redDotCorrect:7;
        unsigned :3;
        unsigned blueDotCorrect:7;
      };
      struct // PWM registers and address bit
      {
        unsigned green:10;
        unsigned red:10;
        unsigned blue:10;
        unsigned command:1;
      };
    } ShiftBritePacket;

    void
      _sendPacket(ShiftBritePacket color),
      _latch(void);

    const uint16_t
      numLEDs,       // Number of RGB LEDs in strip
      numBytes;      // Size of 'pixels' buffer below

    uint8_t latchpin;
    ShiftBritePacket *pixels;
};
#endif // ShiftBrite_h
