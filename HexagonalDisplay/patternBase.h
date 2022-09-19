#pragma once
#include <Arduino.h>
#include "constants.h"

enum Shape{AllOn,Solid,Single,SingleRight,SawLeft,SawRight,SmoothRight,Triangle,Sine,SineFull,SineLeft,Callback};
enum MergeType{MOver,MCopy,MMax,MMedian};
typedef uint32_t (*pixel_callback)(uint8_t ledNumber,uint16_t relativePos);
typedef void (*SimplePatternList[])();


struct patternParams_S{
	uint32_t baseColor;
	int16_t speed;
	Shape shape;
	boolean repeat;
	uint16_t size;
	uint16_t pause;
	uint16_t pausestart;
	uint16_t position; // Start position
	uint8_t intensity;
	boolean gamma;
	MergeType merge;
	pixel_callback callback;
};

typedef struct patternParams_S patternParams;
typedef uint32_t (*pixel_callback)(uint8_t ledNumber,uint16_t relativePos);

void setupPatterns();

void mergeIntoLED(uint32_t baseColor,uint16_t currentLED, uint8_t color1Intensity,MergeType merge,boolean withGamma);
void resetPattern();
void pattern(patternParams &params);
void displayPattern();
void displayPattern(uint8_t patternNumber);
void setBrightness(uint8_t level);

boolean patternInitial();
uint16_t patternPauses();
void patternClearPause();
uint16_t patternRandoms();
void patternRandomize();
uint16_t patternPositions();
void initializeCurrent();