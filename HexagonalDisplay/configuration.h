#include <Adafruit_NeoPixel.h>

#define FASTLED_LIBRARY 0
#define NEOPIXEL_LIBRARY 1
#define ORDER_RGB 1
#define ORDER_GRB 2
#define STRIP_TYPE_WS2811 1

void setupConfiguration();

uint8_t getConfigLedLibrary();
uint8_t getLedStripCount();
uint8_t getConfigDataPin(int stripNumber);
uint16_t getAmountOfLeds();
uint8_t getLedLayout();
uint8_t getLedLayoutFactor();
uint8_t getNumberOfSensors();
boolean isMenuActive();
uint8_t getSensorValue(uint8_t sensorNo);
uint8_t getSensorTreshold(uint8_t sensorNo);
uint8_t getFunctionForSensor(uint8_t sensorNo);
void setFunctionForSensor(uint8_t sensorNo,uint8_t sensorFunction);
int8_t getPatternConfig(uint8_t patternIndex);
uint8_t getFavourite();
uint8_t getTriggerSensorValue();

// Modifiers
void switchLedOrderRed();
void switchLedOrderGreen();
void switchLedOrderBlue();
uint8_t getPixelOrder();
neoPixelType getPixelType();
void setNumberOfLeds(int amount);
void setNumberOfSensors(int amount);
void calibrateSensors();
void configurePatterns();
void switchLedLibrary();