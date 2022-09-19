#include <Arduino.h>

enum ledLayout{layoutSingle,layoutDouble,layoutReverse,layoutQuad};

void setupLeds();
void show();
void ledBrightness(byte brightness);
void allOff();

uint16_t getHue(uint8_t red, uint8_t green, uint8_t blue);
uint32_t getLedColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
uint32_t getLedColorGamma(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
uint32_t getLedColorHSV(uint16_t hue, uint8_t sat, uint8_t val);
void setBar(uint8_t number, uint32_t color);

void pixelhue(uint8_t num, uint16_t huein, uint16_t offset);
void pixelhueStrip(uint8_t strip, uint8_t num, uint16_t huein,uint16_t offset);

void pixelhueStripValue(uint8_t strip, uint8_t num, uint16_t huein,uint16_t offset,int white);
void pixelc(uint8_t num, uint8_t red, uint8_t green, uint8_t blue, uint8_t white);
uint32_t gamma(uint32_t colorIn);
uint32_t getPixelc(uint16_t num);
void pixelcol(uint8_t strip, uint8_t num, uint32_t color);
void pixelcol1(uint8_t num, uint32_t color);
void pixelcol2(uint8_t num, uint32_t color);

uint32_t ledRed();
uint32_t ledBlue();
uint32_t ledYellow();
uint32_t ledGreen();
uint32_t ledBlack();
