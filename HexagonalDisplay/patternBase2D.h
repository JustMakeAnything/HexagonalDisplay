#pragma once
#include "constants.h"
#include "patternBase.h"

// Make sure DISPLAY_X AND DISPLAY_Y are below 16000
#define DISPLAY_X DISPLAY_SIZE_X * DISPLAY_FACTOR
#define DISPLAY_Y DISPLAY_SIZE_Y * DISPLAY_FACTOR
#define MARGIN_X 8192
#define MARGIN_Y 8192

#define CENTERX DISPLAY_SIZE_X * DISPLAY_FACTOR / 2
#define CENTERY DISPLAY_SIZE_Y * DISPLAY_FACTOR / 2

typedef uint32_t (*pixel_callback2D)(uint8_t ledNumber,uint16_t relativePos,uint16_t physX,uint16_t physY);

struct patternParams_S2D{
	uint32_t baseColor;
	int16_t speedx;
	int16_t speedy;
	Shape shape;
	boolean repeat;
	uint16_t size;
	int16_t positionx; // Start position
	int16_t positiony; // Start position
	uint8_t intensity;
	boolean gamma;
	boolean wrap;
	MergeType merge;
	pixel_callback2D callback;
};

typedef struct patternParams_S2D patternParams2D;
//typedef uint32_t (*pixel_callback)(uint8_t ledNumber,uint16_t relativePos);

extern patternParams_S2D patternPDefault2;
extern patternParams2D param2;


uint16_t getPosX();
uint16_t getPosY();
void setPosX(uint16_t posX);
void setPosY(uint16_t posY);

void setInitial2D(boolean initial);
boolean isPatternInitial2d();

uint8_t patternRandoms2D();
uint8_t patternRandoms2D2();
void setRandom2D(uint8_t);
void setRandom2D2(uint8_t);
void patternRandomize2D();

void resetPattern2D();
void resetCurrentPosition2D();
void pattern2D(patternParams2D &params2);

void patternInitialize2D();
void fade2D(uint8_t intensity);