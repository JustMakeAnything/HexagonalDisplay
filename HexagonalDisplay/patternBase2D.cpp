#include <Arduino.h>
#include "constants.h"
#ifdef DEBUG
	#include "debug.h"
#endif
#include "patternBase.h"
#include "patternBase2D.h"
#include "ledStrip.h"
#include "colorTools.h"
#include "fastMath.h"
#include "fastled.h"

uint8_t currentPosition2D = 0;
int16_t positionsX[MAXPOSITIONS];
int16_t positionsY[MAXPOSITIONS];
bool initial2D[MAXPOSITIONS];
uint8_t randoms2D[MAXPOSITIONS];
uint8_t randoms2D2[MAXPOSITIONS];


void resetCurrentPosition2D(){
	currentPosition2D = 0;
}
void resetPattern2D(){
	for (uint8_t position = 0; position < MAXPOSITIONS; position++){
//		positionsX[position] = 0;
//		positionsY[position] = 0;
		initial2D[position] = true;
	}
}

boolean isPatternInitial2d(){
	return initial2D[currentPosition2D];
}

void patternInitialize2D(){
//		positionsX[currentPosition2D] = 0;
//		positionsY[currentPosition2D] = 0;
		initial2D[currentPosition2D] = true;
}

uint16_t getPosX(){
	return positionsX[currentPosition2D];
}
void setPosX(uint16_t posX){
	positionsX[currentPosition2D]=posX;
}
uint16_t getPosY(){
	return positionsY[currentPosition2D];
}
void setPosY(uint16_t posY){
	positionsY[currentPosition2D]=posY;
}
void setInitial2D(boolean initial){
	initial2D[currentPosition2D]=initial;
}

uint8_t patternRandoms2D(){
	return randoms2D[currentPosition2D];
}
uint8_t patternRandoms2D2(){
	return randoms2D2[currentPosition2D];
}
void setRandom2D(uint8_t value){
	randoms2D[currentPosition2D] =value;
}
void setRandom2D2(uint8_t value){
	randoms2D2[currentPosition2D] =value;
}

void patternRandomize2D(){
	randoms2D[currentPosition2D] = random8();
	randoms2D2[currentPosition2D] = random8();
}

void pattern2D(patternParams2D &params){
	// posx,posy = LED position in physical units like mm, can be negative
	// size      = physical size

	if(initial2D[currentPosition2D]==true){
		// Set initial position only once
		positionsX[currentPosition2D]=params.positionx;
		positionsY[currentPosition2D]=params.positiony;
		initial2D[currentPosition2D]=false;
	}

	// Instead of detecting overflow, the 2D has no pauses at end
	// This is needed since the spheres wrap-around to the other side
	// and cannot be removed at a special point seamlessly
    // The coding takes care of this and sets the initial flag 

	// Wrap around
	positionsX[currentPosition2D] += params.speedx;
	if(positionsX[currentPosition2D]< -MARGIN_X){
		// Two step to avoid interger overflow
		positionsX[currentPosition2D] += DISPLAY_X+MARGIN_X;
		positionsX[currentPosition2D] += MARGIN_X;
	}else if (positionsX[currentPosition2D]> DISPLAY_X+MARGIN_X){
		positionsX[currentPosition2D] -= DISPLAY_X+MARGIN_X;
		positionsX[currentPosition2D] -= MARGIN_X;
	}
	positionsY[currentPosition2D] += params.speedy;
	if(positionsY[currentPosition2D]< -MARGIN_Y){
		positionsY[currentPosition2D] += DISPLAY_Y+MARGIN_Y;
		positionsY[currentPosition2D] += MARGIN_Y;
	}else if (positionsY[currentPosition2D] > DISPLAY_Y+MARGIN_Y){
		positionsY[currentPosition2D] -= DISPLAY_Y+MARGIN_Y;
		positionsY[currentPosition2D] -= MARGIN_Y;
	}

	uint16_t patX=positionsX[currentPosition2D];
	uint16_t patY=positionsY[currentPosition2D];

	uint8_t sizex = LED_SIZE_X;
	uint8_t sizey = LED_SIZE_Y;
	uint16_t physX;
	uint16_t physY;
	uint8_t currentLED;

	// LED numbers:              sizex*2-1
	// Input left     y+1/2  odd    5
	// > 0 1 2          0     0          0 +
	//    4 3           1     1          4 -
	//   5 6 7          1     0          5 +
    //    9 8           2     1          9 -

	// Input right    y+1/2  odd                  sx*2-1 * y + (sx-1) -(sx-1)*2*odd
	//   0 1 2 <        0     0          2 -                0  +2
	//    4 3           1     1          3 +                5  -2
	//   5 6 7          1     0          7 -                5  +2
    //    9 8           2     1          8 +               10  -2

// 5 * y + (sx-1)-(sx-1)*2*odd
// 5 * 0 +  2 - 0 = 2
// 5 * 1 +  2 - 4 = 3
// 5 * 1 +  2 - 0 = 7
// 5 * 2 +  2 - 4 = 8


	// Go through all LEDs and see what to do
	for(uint8_t posy = 0; posy < sizey; posy++) {
		uint8_t odd = ((posy)%2); // Odd or even line
		for(uint8_t posx = 0; posx < (sizex-odd); posx++) {
			// Calculate the LED number
			#ifdef INPUTRIGHT
				currentLED = ((posy+1)/2)*(sizex*2-1)+sizex-1-(sizex-1)*2*odd;
			#else
				currentLED = ((posy+1)/2)*(sizex*2-1)-odd;
			#endif
			#ifdef INPUTRIGHT
				if(odd==0){
			#else
				if(odd==1){
			#endif
				currentLED = currentLED-posx;
			}else{
				currentLED = currentLED+posx;
			}
			/*
			Serial.print(currentLED);
			Serial.print("-");
			*/
			// Calculate the physical position
			physX = (uint32_t)posx * DISPLAY_X / (int)(sizex-1);
			physY = (uint32_t)posy * DISPLAY_Y / (int)(sizey-1);
			if(odd==1){
				// Every odd is smaller and moved to right
				physX += DISPLAY_X / 2 / (sizex-1);
			}

//			Serial.print(physY);
//			Serial.print("-");

			uint16_t distance=65535;
			// Check bounding Box
/*
			if( (int16_t)physX>=(int16_t)patX-(int16_t)params.size &&
			    (int16_t)physX<=(int16_t)patX+(int16_t)params.size &&
				(int16_t)physY>=(int16_t)patY-(int16_t)params.size &&
			    (int16_t)physY<=(int16_t)patY+(int16_t)params.size    ){
*/
				distance = calculateDistance(physX,physY,patX,patY);

				if(params.wrap==true){
					// Wrap uses huge amount of performance, but may be needed
					// for special cases
					// Since there is a margin of MARGIN_X,MARGIN_Y around
					// The frame, the movement outside works fine when the object < MARGIN_X or MARGIN_X (16364)
					if(distance>params.size){
						distance = calculateDistance(physX,physY,patX+DISPLAY_X,patY);
					}
					if(distance>params.size){
						distance = calculateDistance(physX,physY,patX-DISPLAY_X,patY);
					}
					if(distance>params.size){
						distance = calculateDistance(physX,physY,patX,patY+DISPLAY_Y);
					}
					if(distance>params.size){
						distance = calculateDistance(physX,physY,patX+DISPLAY_X,patY+DISPLAY_Y);
					}
					if(distance>params.size){
						distance = calculateDistance(physX,physY,patX,patY-DISPLAY_Y);
					}
				}

			uint8_t intensity = (((uint32_t)distance << 8 ) / params.size);
//			Serial.print(intensity);
//			Serial.print("-");

			switch (params.shape){
			case AllOn:
				mergeIntoLED(params.baseColor,currentLED,params.intensity,params.merge,params.gamma);
				break;
			case Solid:
				if(distance < params.size){
					mergeIntoLED(params.baseColor,currentLED,params.intensity,params.merge,params.gamma);
				}
				break;
			case Callback:
				if(distance < params.size){
					mergeIntoLED(params.callback(currentLED,intensity,physX,physY),currentLED,255,params.merge,params.gamma);
				}
				break;

			case Single:{
				// uint16_t ledpos = (pos * (uint32_t)numLeds) >> 16;
				// if(currentLED==ledpos){
				// 	mergeIntoLED(params.baseColor,currentLED,params.intensity,params.merge,params.gamma);
				// }
				// break;
			}
			case SingleRight:{
				int16_t oneLED = DISPLAY_SIZE_X * DISPLAY_FACTOR/ LED_SIZE_X +100;
				if((distance < params.size && (int)distance>(int)params.size-oneLED)){
					mergeIntoLED(params.baseColor,currentLED, 255,params.merge,params.gamma);
				}
				break;
			}
			case SawLeft:{
				// In 2D SawLeft is a linear drop from the center
				if(distance < params.size){
					intensity = 255-intensity;
					intensity = (uint16_t)intensity * params.intensity / 255;
					mergeIntoLED(params.baseColor,currentLED, intensity,params.merge,params.gamma);
				}
				break;
			}
			case SawRight:{
				// In 2D SawRight is a linear increase from the center forming a ring
				if(distance < params.size){
					mergeIntoLED(params.baseColor,currentLED, intensity,params.merge,params.gamma);
				}
				break;
			}
			case Sine:{
				uint8_t strength;
				if(distance<params.size){
					if(intensity<128){
						strength = ease8InOutQuad((intensity)*2);
					}else{
						strength = ease8InOutQuad((255-intensity)*2);
					}
					mergeIntoLED(params.baseColor,currentLED,strength,params.merge,params.gamma);
				}
				break;
			}
			case SineLeft:{
				uint8_t strength;
				if(distance<params.size){
						strength = ease8InOutQuad(255-intensity);
					mergeIntoLED(params.baseColor,currentLED,strength,params.merge,params.gamma);
				}
				break;
			}
			case SmoothRight:{
				uint8_t strength;
				intensity=255-intensity;
				if(distance<params.size){
					// Some virtual value between 32 and 127
					// 32 = very thin
					// 127 = max thickness
					int thickness=64;
					// size= 8000 thickness = 32
					// size= 1000 thickness = 127
					// Maybe it needs some tuning?
					thickness = 19100/max(min(params.size/10,400),100);
//					Serial.print(thickness);
					// First Quarter always the same
					if(intensity<32){  
						// Steep up
						strength = ease8InOutQuad((intensity)*8);
					// Then some variable sized always on
					}else if(intensity<thickness+32){
						strength = 255;
					// Then a quarter at variable position
					}else if(intensity<thickness+64){
						// 64= 255 128=0 
						strength = ease8InOutQuad((thickness+64-intensity)*8);
					}else{
						// Dark inside (The rest)
						strength = 0;
					}
					if(params.size<900){
						strength=255;
					}
					mergeIntoLED(params.baseColor,currentLED,strength,params.merge,params.gamma);
				}
				break;
			}


			default:
				mergeIntoLED(params.baseColor,currentLED,params.intensity,params.merge,params.gamma);
				break;
			}
		}
		
	}
//	Serial.println();
	currentPosition2D++;
}

void fade2D(uint8_t intensity){
	param2 = patternPDefault2;
	param2.baseColor = getLedColor(0,0,0,0); 
	param2.shape = AllOn;
	param2.merge = MOver;
	param2.size  = 65535;
	param2.intensity = intensity;
	pattern2D(param2);
}
