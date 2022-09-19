#include <Arduino.h>
#include "colorTools.h"
#include <FastLED.h>

CRGBW& nblend(CRGBW& existing, const CRGBW& overlay, uint8_t amountOfExisting )
{
    if( amountOfExisting == 255) {
        return existing;
    }
    if( amountOfExisting == 0) {
        existing = overlay;
        return existing;
    }
    existing.red   = blend8( existing.red,   overlay.red,   255-amountOfExisting);
    existing.green = blend8( existing.green, overlay.green, 255-amountOfExisting);
    existing.blue  = blend8( existing.blue,  overlay.blue,  255-amountOfExisting);
    existing.white = blend8( existing.white, overlay.white, 255-amountOfExisting);
    return existing;
}
