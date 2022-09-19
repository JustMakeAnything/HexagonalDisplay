#include <Arduino.h>
#include "constants.h"
#ifdef DEBUG
#include "debug.h"
#endif
#include "touch.h"
#include "patterns.h"
#include "patternBase.h"
#include "ledStrip.h"
#include "colorTools.h"
#include "fastMath.h"
#include "fastled.h"
#include "configuration.h"
#include "patterns2D.h" // For 2D patterns
#include "patternInteractive2D.h"
#pragma GCC diagnostic ignored "-Wunused-parameter"

static uint16_t numLeds = 10;
patternParams_S patternPDefault = {0,0,Single,false,0,0,0,0,255,false,MOver,NULL};
patternParams param; // Reusable to save memory


void fade(uint8_t intensity){
	param = patternPDefault;
	param.baseColor = getLedColor(0,0,0,0); 
	param.shape = AllOn;
	param.merge = MOver;
	param.size  = 65535;
	param.intensity = intensity;
	pattern(param);
}

// List of patterns to cycle through.  
// Each is defined as a separate function below.
// There is a limit of patterns in ...
SimplePatternList patternList = { 
	patternRainbow2D,
	patternCloudySky2DParam,
	patternFire2D,
	patternTunnel2D,
	patternPlasma2D,
	patternRainbowCallback,
//	patternInteractive2D,
};

void executePattern(uint8_t patternNumber){
	patternList[patternNumber]();
}


#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

uint8_t getPatternCount(){
	return min(ARRAY_SIZE(patternList),MAXPATTERNS);
}

void patternConfigure(){
	allOff();
	pixelc(0,255,0,0,0); // Red
	pixelc(1,0,255,0,0); // Green
	pixelc(2,0,0,255,0); // Blue
	pixelc(3,0,0,0,255); // White
	pixelc(4,255,255,255,255); // All RGB+White
}

void patternLeftRight(){
	fade(20);
	param = patternPDefault;
	param.baseColor = getLedColor(255,0,0,0); 
	param.shape = Single;
	param.merge = MMax;
	param.intensity = 255;
	param.speed = 500;
	param.size  = 15000;
	param.pause =  65535;
	pattern(param);
	param.pausestart = 65535;
	param.speed = -500;
	param.baseColor = getLedColor(0,0,255,0); 
	pattern(param);
}

void patternCloudySky(){
	allOff();
	param = patternPDefault;
	param.baseColor = getLedColor(0,0,150,0); // Dark blue Sky
	param.shape = AllOn;
	param.merge = MMax;
	param.intensity = 255;
	pattern(param);

	// Big cloud, litte cyan
	param.baseColor = getLedColor(100,200,200,0); 
	param.shape = Sine;
	param.size = 30000;
	param.merge = MMax;
	param.speed = 50;
	pattern(param);

	// Medium Cloud, White
	param.baseColor = getLedColor(128,128,128,0);
	param.size = 20000;
	param.speed = -40;
	pattern(param); 

	// Smaller Clouds with white Leds
	param.baseColor = getLedColor(0,0,0,150);
	param.size = 10000;
	param.speed = 90;
	pattern(param);

	param.size = 8000;
	param.speed = -70;
	pattern(param);

}

#define riseSpeed 300
uint32_t risen = 0;
uint8_t patternSunriseNewHelp1(uint32_t offset){
	param.size = min(risen-offset,65500);
	param.speed = -riseSpeed>>1;
	if(param.size>=65500){
		param.speed =0;
	}
	return min(param.size/100,200);
}
void patternSunriseNewHelp2(uint32_t offset){
	if(risen>offset){
		pattern(param);
	}	
}
void patternSunriseNew(){
	allOff();
	if(patternInitial()){
		risen = 0;
	}
	param = patternPDefault;
	param.shape = Sine;
	param.merge = MMax;
	param.position=32768;
	risen +=riseSpeed;
	param.baseColor = getLedColor(patternSunriseNewHelp1(0),0,0,0);
	patternSunriseNewHelp2(0);
	if(risen>=120000){
		risen=120000;
	}
	param.baseColor = getLedColor(0,patternSunriseNewHelp1(15000),0,0);
	patternSunriseNewHelp2(15000);

	param.baseColor = getLedColor(0,0,patternSunriseNewHelp1(35000),0);
	patternSunriseNewHelp2(35000);

	param.shape = AllOn;
	if(risen>70000){
		uint8_t bright = ((risen-70000)*255)/(120000-70000)/2;
		param.baseColor = getLedColorGamma(0,0,0,bright);
		pattern(param);
	}
}

void patternRedWhite(){
	allOff();
	param = patternPDefault;
	param.shape = Sine;
	param.repeat = true;
	param.baseColor = getLedColor(255,0,0,0);
	param.merge = MMax;
	param.size = 65536/numLeds*2;
	param.speed = 10;
	pattern(param);	
	param.gamma = true; // Gamma of white is different from red
	param.baseColor = getLedColor(0,0,0,255);
	param.position = 65536/numLeds;
	pattern(param);	

}

uint32_t callbackRainbow(uint8_t ledNumber,uint16_t relativePos){
	return getLedColorHSV(relativePos,255,255);
}

void patternRainbowCallback(){
	allOff();
	param = patternPDefault;
	param.shape = Callback;
	param.merge = MOver;
	param.callback = callbackRainbow;
	param.size = 65535;
	param.speed = 200;
	pattern(param);	
}

// Pattern examples
void patternGenMulti(){
	allOff();
	param = patternPDefault;
	param.baseColor = getLedColor(255,255,0,0);
	param.speed = 100;
	param.shape = Single;
	pattern(param);

	param.baseColor = getLedColor(255,0,0,0);
	param.shape = SawLeft;
	param.speed = 200;
	param.size = 5000;
	pattern(param);

	param.baseColor = getLedColor(0,0,255,0);
	param.shape = SawRight;
	param.speed = -300;
	param.size = 15000;
	pattern(param);
}


void sunriseSingle(uint8_t bar,float h){

	h = max(h,0);
	float hsqrt = ( float(sqrt( h )));
	int red   = ( hsqrt * hsqrt );
	int green = ( h * hsqrt ) / 256 * 16.0;
	int blue  = ( h * h ) / 256.0;
	int white = 0;
	if(h>130){
		red -= (h-130);
		green -= (h-130);
		blue -= (h-130);
	}
	setBar(bar,getLedColor(red,green,blue,white));
}
void patternDrops(){
	fade(100);
	param = patternPDefault;
	param.shape = Single;
	param.merge = MMax;
	param.pause = 10000;
	param.intensity = 255;
	
	random16_set_seed(micros()%65536);
	for (uint8_t i = 0; i < 4; i++){
		if(patternInitial() || patternPauses()>0){
			patternRandomize();
			patternClearPause();
		}
		if(patternRandoms()>127){
			param.baseColor = getLedColorHSV(uint16_t(patternRandoms())*64+256*96,255,255);
		}else{
			param.baseColor = getLedColorHSV(uint16_t(patternRandoms())*64,255,255);
		}
		param.pausestart = i*5000+100;
		param.speed = patternRandoms()+40+(patternPositions()>>7);
		pattern(param);	
	}
}
void patternFire(){
	fade(20);
	param = patternPDefault;
	param.shape = Single;
	param.merge = MMax;
	param.pause = 10000;
	param.intensity = 255;
	
	random16_set_seed(micros()%65536);
	for (uint8_t i = 0; i < MAXPOSITIONS-1; i++){
		if(patternInitial() || patternPauses()>0){
			patternRandomize();
			patternClearPause();
		}
		param.baseColor = getLedColorHSV(uint16_t(patternRandoms())*30,255,255);
		param.intensity = max(0,255-((int)(patternPositions()>>7)-((int)patternRandoms()/2)));
		param.pausestart = i*3000+100;
		param.speed = patternRandoms()+100+(patternPositions()>>5);
		pattern(param);	
	}
}
