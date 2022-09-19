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
#include "configuration.h"
#include "patternInteractive2D.h"

// Interactive pattern. Use keyboard to control
// You need to deactivate the menu, otherwise the
// keyboard entry will not work reliably
//
// Keys:
// WSAD: Change speed
// Q/E : Change size
// R   : Change shape
// Open:

patternParams2D paramI;
void patternInteractive2D(){
//	allOff();
	fade2D(20);

	if(isPatternInitial2d()){
		param2 = patternPDefault2;
		param2.baseColor = getLedColor(0,255,100,0);
		param2.shape = Solid;
		param2.merge = MMax;
		param2.size  = 4000;
		param2.speedx = 0;
		paramI=param2;
	}
	if(Serial.available()){
		int command = Serial.read();
//		Serial.print(command);
		switch (command){
		case 119: //'W'
			paramI.speedy+=10;
			Serial.print("SpeedY: ");
			Serial.print(paramI.speedy);
			break;
		case 115: //'S'
			paramI.speedy-=10;
			Serial.print("SpeedY: ");
			Serial.print(paramI.speedy);
			break;
		case 97: //'A'
			paramI.speedx-=10;
			Serial.print("SpeedX: ");
			Serial.print(paramI.speedx);
			break;
		case 100: //'D'
			paramI.speedx+=10;
			Serial.print("SpeedX: ");
			Serial.print(paramI.speedx);
			break;
		case 113: //'Q'
			paramI.size-=100;
			Serial.print("Size: ");
			Serial.print(paramI.size);
			break;
		case 101: //'E'
			paramI.size+=100;
			Serial.print("Size: ");
			Serial.print(paramI.size);
			break;
		case 114: //'R'
			do{

				Serial.print("Shape: ");
				if(paramI.shape==Callback){
					paramI.shape=AllOn;
				}
				switch (paramI.shape)
				{
				case SmoothRight:
					paramI.shape = Solid;
					Serial.println("Solid");
					break;
				case Solid:
					paramI.shape = SawLeft;
					Serial.println("SawLeft");
					break;
				case SawLeft:
					paramI.shape = SawRight;
					Serial.println("SawRight");
					break;
				case SawRight:
					paramI.shape = SingleRight;
					Serial.println("SingleRight");
					break;
				case SingleRight:
					paramI.shape = Sine;
					Serial.println("Sine");
					break;
				case Sine:
					paramI.shape = SineLeft;
					Serial.println("SineLeft");
					break;
				case SineLeft:
					paramI.shape = SmoothRight;
					Serial.println("SmoothRight");
					break;
				
				default:
					break;
				}
			// Only use the patterns which are usable for 2D
			} while (paramI.shape!=Solid &&
			         paramI.shape!=SawLeft && 
					 paramI.shape!=SawRight &&
				     paramI.shape!=SingleRight && 
					 paramI.shape!=Sine &&
					 paramI.shape!=SineLeft &&
					 paramI.shape!=SmoothRight );
			break;
		
		default:
			break;
		}
		Serial.println();
	}
	pattern2D(paramI);
}
