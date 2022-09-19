#include <Arduino.h>
#include "constants.h"
#ifdef DEBUG
#include "debug.h"
#endif
#include "patternBase.h"
#include "patterns.h"
#include "ledStrip.h"
#include "colorTools.h"
#include "fastMath.h"
#include "configuration.h"
#include "patternBase2D.h"
#include "state.h"
#include "fastled.h"

static byte brightness = 0;
static uint16_t numLeds = 10;

bool initial[MAXPOSITIONS] = {true,true,true,true,true,true,true};
uint8_t currentPosition = 0;
uint16_t positions[MAXPOSITIONS]={0,0,0,0,0,0,0};
uint16_t pauses[MAXPOSITIONS]={0,0,0,0,0,0,0};
uint8_t randoms[MAXPOSITIONS];

boolean off;

typedef struct patternParams_S patternParams;

////////////////////////////////////////

void setupPatterns(){
	setupLeds();
	numLeds = getAmountOfLeds()/getLedLayoutFactor();
}


void resetPattern(){
	for (uint8_t position = 0; position < MAXPOSITIONS; position++){
		positions[position] = 0;
		pauses[position] = 0;
		initial[position] = true;
	}
	resetPattern2D();
}

void initializeCurrent(){
		initial[currentPosition] = true;
}

void setBrightness(uint8_t level){
	#ifdef debug
		//debug("Bright: ");
		//debugn(level);
	#endif

	switch (level)
	{
	case 0:
		brightness = 20;
		break;
	case 1:
		brightness = 100;
		break;
	case 2:
		brightness = 255;
		break;
	default:
		brightness = 30;
		break;
	}
	ledBrightness(brightness);
}

void displayPattern(){
	displayPattern(getPatternNumber());
}

void displayPattern(uint8_t patternNumber){
	setBrightness(getBrightLevel());
    #ifdef DEBUG
	#ifdef LOWMEM
	debug('P',"P: ");
	debug('P',patternNumber);
	#else
	debug('P',"Pattern: ");
	debug('P',patternNumber);
	#endif
	#endif
	if(patternNumber>90){
		// Special patterns
		switch (patternNumber){
// 		case 98:
// //			patternSunrise();
// 			break;
// 		case 99:
// //			patternSunset();
// 			break;
		case 100: 
			// Do nothing, keep existing, or let other code take control
			// Don't even call show();
		    return;
		case 101:
			patternConfigure();
			break;
		case 102:
			patternLeftRight();
			break;
		default:
			allOff();
			break;
		}

	}else{
		currentPosition = 0;
		resetCurrentPosition2D();
		executePattern(patternNumber);
	}

	show();
}

boolean patternInitial(){
	return initial[currentPosition];
}
uint16_t patternPauses(){
	return pauses[currentPosition];
}
void patternClearPause(){
	pauses[currentPosition] = 0;
}
uint16_t patternRandoms(){
	return randoms[currentPosition];
}
void patternRandomize(){
	randoms[currentPosition] = random8();
}
uint16_t patternPositions(){
	return positions[currentPosition];
}


void mergeIntoLED(uint32_t baseColor,uint16_t currentLED, uint8_t color1Intensity,MergeType merge,boolean withGamma){
	CRGBW color = CRGBW(baseColor);
	CRGBW color1 = CRGBW(color);
	CRGBW prevColor = getPixelc(currentLED);
	switch (merge)
	{
	case MCopy:
		// 1:1, no blending according to Size
		color1 = baseColor;
    break;
	case MOver:
		color1 = nblend(color,prevColor,color1Intensity);
		break;
	case MMax:
		color = nblend(color,CRGBW(0,0,0,0),color1Intensity);
		color1.r = max(color.r,prevColor.r);
		color1.g = max(color.g,prevColor.g);
		color1.b = max(color.b,prevColor.b);
		color1.w = max(color.w,prevColor.w);
		break;
		/*
	case MMin:
		color = nblend(color,CRGBW(0,0,0,0),color1Intensity);
		color1.r = max(color.r,prevColor.r);
		color1.g = max(color.g,prevColor.g);
		color1.b = max(color.b,prevColor.b);
		color1.w = max(color.w,prevColor.w);
		break;
		*/
	case MMedian:
		color = nblend(color,CRGBW(0,0,0,0),color1Intensity);
		color1.r = (color.r>>1)+(prevColor.r>>1);
		color1.g = (color.g>>1)+(prevColor.g>>1);
		color1.b = (color.b>>1)+(prevColor.b>>1);
		color1.w = (color.w>>1)+(prevColor.w>>1);
		break;
	default:
		color1 = nblend(color,prevColor,color1Intensity);
		break;
	}
	uint32_t col;
	col =  *reinterpret_cast<uint32_t*>(&color1);
	if(withGamma){
		col = gamma(col);
	}
	setBar(currentLED,col);
}


void pattern(patternParams &params){
	// baseColor = RGBW
	// pos       = 0% - 100% encoded as 0-65535   1% is about 655
	// size      = 0% - 100% encoded as 0-65535   Maximum extension of effect
	// pause     = 0% - 100% encoded as 0-65535   A pause can be defined up to 
	// ledpos = 0 to 256 * maxled. The sub-led accuracy is therefore 1/256
	// Position is automatically managed by the sequence the pattern method is called
	if(initial[currentPosition]==true){
		pauses[currentPosition]=params.pausestart;
		positions[currentPosition]=params.position;
		initial[currentPosition]=false;
	}
	int32_t overflow;
	if(params.pause>0 || pauses[currentPosition]>0){ // Start pause also works when no pause is defined

		overflow = pauses[currentPosition];
		if(overflow>0){
			// It is currently pause
			overflow -= abs(params.speed);
			if(overflow < 0){
				pauses[currentPosition] = 0;
				positions[currentPosition] -= overflow * sgn(params.speed);
			}else{
				pauses[currentPosition]-=abs(params.speed);
				// Execute the pause
				currentPosition++;
				return;
			}
		}else{
			// Detect overflow
			overflow = positions[currentPosition];
			overflow += params.speed;
			if(overflow<0){
				pauses[currentPosition] = params.pause+overflow;
				positions[currentPosition]=0;
				currentPosition++;
				return;
			}else if(overflow > 65535){
				pauses[currentPosition] = params.pause-(overflow-65535);
				positions[currentPosition]=0;
				currentPosition++;
				return;
			}else{
				positions[currentPosition] += params.speed;
			}
		}
	}else{
			positions[currentPosition] += params.speed;

	}
	uint16_t pos = positions[currentPosition]; 

	for(uint16_t currentLED = 0; currentLED < numLeds; currentLED++) {

		uint16_t currentPos = (uint32_t)( (uint32_t)currentLED << 16 ) / numLeds;
		uint16_t relativePos = currentPos - pos;
		uint8_t  relativeToSize = (uint32_t)relativePos*255/(params.size);


		switch (params.shape){
		case AllOn:
			mergeIntoLED(params.baseColor,currentLED,params.intensity,params.merge,params.gamma);
			break;
		case Callback:
			mergeIntoLED(params.callback(currentLED,relativePos),currentLED,255,params.merge,params.gamma);
			break;

		case Single:{
			uint16_t ledpos = (pos * (uint32_t)numLeds) >> 16;
			if(currentLED==ledpos){
				mergeIntoLED(params.baseColor,currentLED,params.intensity,params.merge,params.gamma);
			}
			break;
		}
		case Solid:{
			if(relativePos<params.size || params.repeat == true){
				relativePos %= params.size;
				mergeIntoLED(params.baseColor,currentLED,255,params.merge,params.gamma);
			}
			break;
		}
		case SawRight:{ 
			// 
			if(relativePos<params.size || params.repeat == true){
				relativePos %= params.size;
				mergeIntoLED(params.baseColor,currentLED,255-relativeToSize,params.merge,params.gamma);
			}
			break;
		}
		case SawLeft:{
			// 
			if(relativePos<params.size){
				mergeIntoLED(params.baseColor,currentLED,relativeToSize,params.merge,params.gamma);
			}
			break;
		}
		case Sine:{
			uint8_t strength;
			if(relativePos<params.size || params.repeat == true){
				relativePos %= params.size;
				if(relativeToSize<128){
					strength = ease8InOutQuad((relativeToSize)*2);
				}else{
					strength = ease8InOutQuad((255-relativeToSize)*2);
				}
				mergeIntoLED(params.baseColor,currentLED,strength,params.merge,params.gamma);
			}
			break;
		}
		case SineFull:{
			uint8_t strength;
			if(relativePos<params.size || params.repeat == true){
				relativePos %= params.size;
				if(relativeToSize<64){
					strength = 128-ease8InOutQuad((255-relativeToSize*4))/2;
				}else if(relativeToSize<128){
					strength = 128+ease8InOutQuad(((relativeToSize-64)*4))/2;
				}else if(relativeToSize<192){
					strength = 128+ease8InOutQuad(((191-relativeToSize)*4))/2;
				}else{
					strength = 128-ease8InOutQuad((relativeToSize-192)*4)/2;
				}
				mergeIntoLED(params.baseColor,currentLED,strength,params.merge,params.gamma);
			}
			break;
		}
		default:
			setBar(currentLED,ledBlue());
			break;
		}
		
	}
	currentPosition++;
}
