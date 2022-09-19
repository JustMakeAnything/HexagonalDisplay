#include <Arduino.h>
#include "constants.h"
#ifdef DEBUG
#include "debug.h"
#endif
#include "touch.h"
#include "patterns.h"
#include "patterns2D.h"
#include "patternBase.h"
#include "patternBase2D.h"
#include "ledStrip.h"
#include "colorTools.h"
#include "fastMath.h"
#include "fastled.h"
#include "configuration.h"
#pragma GCC diagnostic ignored "-Wunused-parameter"


patternParams_S2D patternPDefault2 = {0,0,0,Single,false,0,CENTERX,CENTERY,255,false,false,MOver,NULL};
patternParams2D param2; // Reusable to save memory

// The patterns which you want to use
// need to be listed in patterns.cpp
// int the SimplePatternList patternList array


void patternAround(){
	fade2D(20);
	param2 = patternPDefault2;
	param2.baseColor = getLedColor(0,255,0,0); 
	param2.shape = SawLeft;
//	param2.shape = AllOn;
	param2.merge = MMax;
	param2.intensity = 255;
	param2.speedx = 20;
	param2.speedy = 30;
	param2.size  = 2000;
	pattern2D(param2);

	// Blue around
	param2.positionx = CENTERX;
	param2.positiony = CENTERY;
	param2.shape = SawRight;
	param2.size  = 3000;
	param2.speedx = -10;
	param2.speedy = 20;
	param2.baseColor = getLedColor(0,0,255,0); 
	pattern2D(param2);

	// Red inside
	param2.shape = SawLeft;
	param2.size  = 2000;
	param2.speedx = -10;
	param2.speedy = 20;
	param2.baseColor = getLedColor(255,0,0,0); 
	pattern2D(param2);



// Gamma all.
	param2.baseColor = getLedColor(0,0,0,0); 
	param2.intensity = 0;
	param2.shape = AllOn;
	param2.gamma = true;
	pattern2D(param2);
}

uint16_t rainbowCycle = 0;
uint32_t callbackRainbow2D(uint8_t ledNumber __attribute__((unused)),uint16_t relativePos,uint16_t physX, uint16_t physY){
	rainbowCycle--;
	return getLedColorHSV(relativePos*80+rainbowCycle,255,255);
}

void patternRainbow2D(){
	allOff();	
	param2 = patternPDefault2;
	param2.merge = MOver;
	param2.size  = 6000;
	param2.shape = Callback;
	param2.speedx = 0;
	param2.speedy = 0;
	param2.positionx = CENTERX;
	param2.positiony = CENTERY;
	param2.callback = callbackRainbow2D;
	pattern2D(param2);
}

void patternCloudySky2DParam(){
//	allOff();
	fade2D(4);
	param2 = patternPDefault2;
	param2.baseColor = getLedColor(0,0,90,0); // Dark blue Sky
	param2.shape = AllOn;
	param2.merge = MMax;
	param2.intensity = 255;
	pattern2D(param2);


	int16_t rand1;
	int16_t rand2;
	random16_set_seed(micros()%65536);
	for (uint8_t i = 0; i < 6; i++){ // 6 Clouds
		if(isPatternInitial2d()){
			patternRandomize2D(); // Get some randoms			
		}
		rand1 = patternRandoms2D();
		rand2 = patternRandoms2D2();
		// Create a cloud from 2 Values (Random/Random2)
		uint8_t baseCol = rand1/3+128;
		param2.baseColor = getLedColor(baseCol,baseCol+rand2/8,min(255,baseCol+rand2/4),0); 
		param2.shape = SawLeft;
		param2.size = ((int)rand1)*11+((int)rand2)*4+1000;
		param2.merge = MOver;
		param2.intensity = 60;
		param2.positionx = rand1*100;
		param2.positiony = rand2*100;
		param2.speedx = (rand1-96)/8;
		param2.speedy = (rand2-96)/8;

		if(random8()>128 && rand1<200){
			rand1+=1;
		}else if(rand1>50){
			rand1-=1;
		}
		if(random8()>128 && rand2<200){
			rand2+=1;
		}else if(rand2>50){
			rand2-=1;
		}
		setRandom2D(rand1);
		setRandom2D2(rand2);
		
		pattern2D(param2);
		/*
		Serial.print(" r1 ");
		Serial.print(rand1);
		Serial.print(" r2 ");
		Serial.print(rand2);
		Serial.println();
		*/

	}
}


uint8_t particles = 8;
void patternFire2D(){
	// Keep trails of fire particles
	fade2D(15);
	param2 = patternPDefault2;
	param2.shape = SawLeft; // Circle fade at edge
	param2.merge = MMax;    // Add up
	param2.intensity = 255;
	//setSeed(micros()%65536);
	/*
	if(Serial.available()){
		int command = Serial.read();
		switch (command){
		case 97: //'A'
			particles--;
			break;
		case 100: //'D'
			particles++;
			break;
		}
		if(particles>MAXPOSITIONS-1){
			particles = MAXPOSITIONS-1;
		}
		Serial.print("Particles: ");
		Serial.println(particles);
	}
	*/

	for (uint8_t i = 0; i < min(8,particles); i++){ // 8 Particles when available
		int ypos = (uint32_t)getPosY()*64/CENTERY; // 0-255 bottom to top
		// Set initial state for fire
		if(isPatternInitial2d()){
			setPosX(CENTERX);                     // Middle of screen
			setPosY(i*CENTERY/MAXPOSITIONS+2000); // A vertical column
			setInitial2D(false);                  // Do not initialize automatically
			patternRandomize2D();                 // Get some randoms
		}
        // Yellow to red
		int random = patternRandoms2D();
		param2.baseColor = getLedColorHSV((getTriggerSensorValue()/4+350-(ypos))*22,220,255);
		param2.intensity = max(0,255-(int)(ypos));   // Fade to the top
		param2.speedy = 50+(getPosY()>>3);//+getTriggerSensorValue();           // Accelerate to the top
		param2.speedx = (  (int)(random-128)*220/ypos)  // Widen at the bottom 
					     -((int)(random-128)*(ypos/40));// Narrow to the top
		param2.positionx = CENTERX + ((int)random-128)*4; // Start somewehere in the middle
		param2.positiony = CENTERY/15;                           // Start at bottom
		param2.size = 700;                                       // Fire particle size
		// Remove particle at top. Some random helps to make it more lively
		if(getPosY()>(uint16_t)CENTERY*5/3 - random*8 + getTriggerSensorValue()*4){
			patternRandomize2D();
			patternInitialize2D();
		}
		pattern2D(param2);
	}
}


uint32_t tunnelPos;
uint32_t HUEStart=0;
void patternTunnel2D(){
	fade2D(50);
	param2 = patternPDefault2;
	// Circular tunnel elements
	param2.positionx = CENTERX;
	param2.positiony = CENTERY;
	param2.shape = SmoothRight;//SingleRight;
	param2.size  = tunnelPos % (DISPLAY_SIZE_Y*DISPLAY_FACTOR / 3);
	param2.speedx = 0;
	param2.speedy = 0;
 	
	param2.baseColor = getLedColorHSV(tunnelPos % (DISPLAY_SIZE_Y*DISPLAY_FACTOR / 3)*3+HUEStart,255,255);	
	pattern2D(param2);
	param2.baseColor = getLedColorHSV(tunnelPos % (DISPLAY_SIZE_Y*DISPLAY_FACTOR / 3)*3+10290+HUEStart,255,255);	
	param2.size  = tunnelPos % (DISPLAY_SIZE_Y*DISPLAY_FACTOR / 3)+DISPLAY_SIZE_Y*DISPLAY_FACTOR / 3;
	pattern2D(param2);


	tunnelPos+=180;
	if(tunnelPos>DISPLAY_SIZE_Y*DISPLAY_FACTOR/1.5*20){
		tunnelPos=0;
	}
	HUEStart+=10;
}


float sec;
uint32_t callbackPlasma2D(uint8_t ledNumber,uint16_t relativePos,uint16_t physX,uint16_t physY){

// https://rosettacode.org/wiki/Plasma_effect#C

	int16_t oneLED = DISPLAY_SIZE_X * DISPLAY_FACTOR/ LED_SIZE_X;

	double dx,dy,dv;
	float posX,posY;
	#define pi 3.14159

	posX = (float)physX/oneLED/70.0;
	posY = (float)physY/oneLED/70.0;

	dx = posX + .5 * sin(sec/5.0);
	dy = posY + .5 * cos(sec/3.0);
			
	dv = sin(posX*10 + sec) + 
	     sin(10*(posX*sin(sec/2.0) + 
		 posY*cos(sec/3.0)) + sec) + 
		 sin(sqrt(100*(dx*dx + dy*dy)+1) + sec);

	#define darker 80.0
	#define darkcorr 30.0
	return getLedColor(max((255+darkcorr)*fabs(sin(dv*pi))-darker,0),
				max((255+darkcorr)*fabs(sin(dv*pi + 2*pi/3))-darker,0),
				max((255+darkcorr)*fabs(sin(dv*pi + 4*pi/3))-darker,0),0);

}
/*
      leds[XY(x, y)] = ColorFromPalette(
          palette,
          beatsin8(5, 0, 42, 0, x*8)
          + beatsin8(11, 0, 42, 0, -x*9)

          + beatsin8(9, 0, 42, 0, y*10)
          + beatsin8(13, 0, 42, 0, -y*11)

          + beatsin8(7, 0, 42, 0, (x + y)*12)
          + beatsin8(15, 0, 42, 0, -(x + y)*13)
        ,255
       );
	   */


void patternPlasma2D(){
	allOff();	
	param2 = patternPDefault2;
	param2.merge = MOver;
	param2.size  = 6000;
	param2.shape = Callback;
//	param2.speedx = 0;
//	param2.speedy = 0;
//	param2.positionx = CENTERX;
//	param2.positiony = CENTERY;
	param2.callback = callbackPlasma2D;
	pattern2D(param2);
	sec+=0.015;

}
