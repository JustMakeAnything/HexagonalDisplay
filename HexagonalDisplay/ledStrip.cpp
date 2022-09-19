#include <Arduino.h>
#include "constants.h"
#ifdef DEBUG
#include "debug.h"
#endif
#include "ledStrip.h"
#include "configuration.h"
#ifdef FASTLED
	#include <FastLED.h>
#endif
#include <Adafruit_NeoPixel.h>

uint8_t ledStripCount = MAX_LED_STRIP_COUNT;

#ifdef FASTLED
	CRGBArray<MAX_NUM_LEDS> leds[MAX_LED_STRIP_COUNT];
#endif
Adafruit_NeoPixel LEDs[MAX_LED_STRIP_COUNT];

//bool ledLibrary;
uint8_t NumberLeds;
uint8_t layout;
uint8_t layoutFactor;

#define LED_TYPE   WS2811
#define COLOR_ORDER   GRB
#define DATA_PIN        3

void setupLeds(){

	NumberLeds = getAmountOfLeds() / getLedLayoutFactor();
	ledStripCount = getLedStripCount();
	layout = getLedLayout();
	layoutFactor = getLedLayoutFactor();
//	ledLibrary = getConfigLedLibrary();
	#ifdef DEBUG
	#ifdef LOWMEM
		debug('L',"LEDs: ");
		debugn('L',NumberLeds);
		debug('L',"Strips: ");
		debugn('L',ledStripCount);
		#ifdef FASTLED
		debug('L',"Led Lib: ");
		debugn('L',ledLibrary);
		#endif
	#else
		debug('L',"Number of LEDs: ");
		debugn('L',NumberLeds);
		debug('L',"Led strip count: ");
		debugn('L',ledStripCount);
		debug('L',"Led Library: ");
		debugn('L',ledLibrary);
	#endif
	#endif

// Powersaving currently not implemented
//	pinMode(LED_ENABLE, OUTPUT);
//	digitalWrite(LED_ENABLE,HIGH);
	for (uint8_t stripNumber = 0; stripNumber < ledStripCount; stripNumber++){
		uint8_t ledStripDataPin;
		if(stripNumber==0){
			ledStripDataPin = DATA_PIN1;
		}else{
			ledStripDataPin = DATA_PIN2;
		}		
		#ifdef FASTLED
			switch (stripNumber){
			case 0:
				ledStripDataPin = getConfigDataPin(1);
				if(ledLibrary==FASTLED_LIBRARY){
	//	        	FastLED.addLeds<LED_TYPE,DATA_PIN1,COLOR_ORDER>(leds[stripNumber], NumberLeds);
				}
				break;
			case 1:
				ledStripDataPin = DATA_PIN2;
				if(ledLibrary==FASTLED_LIBRARY){
	//	        	FastLED.addLeds<LED_TYPE,DATA_PIN2,COLOR_ORDER>(leds[stripNumber], NumberLeds);
				}
				break;
			default:
				#ifdef DEBUG			
				debug('C',"Serial pin error ");
				#endif
				break;
			}
			if(ledLibrary==FASTLED_LIBRARY){
				#ifdef FASTLED
					FastLED.setCorrection(TypicalLEDStrip);
				#endif
			}else if(ledLibrary==NEOPIXEL_LIBRARY){
		#endif
//			LEDs[stripNumber] = Adafruit_NeoPixel(NumberLeds, ledStripDataPin, getPixelType());
			LEDs[stripNumber] = Adafruit_NeoPixel(getAmountOfLeds(), ledStripDataPin, getPixelType());
//			LEDs[stripNumber] = Adafruit_NeoPixel(getAmountOfLeds(), ledStripDataPin, STRIP_TYPE+NEO_KHZ800);
			LEDs[stripNumber].begin();
			allOff();
			#ifdef DEBUG
			#ifdef FASTLED
			debug('C',"Neopixel ");
			#endif
			#endif
		#ifdef FASTLED
		}else{
			#ifdef DEBUG
			debug('C',"Unkown Library: ");
			debugn('C',ledLibrary);
			#endif
		}
		#endif
		ledBrightness(50);
		show();
	}
}

uint32_t getLedColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w){
	#ifdef FASTLED
		if(ledLibrary==FASTLED_LIBRARY){
			return CRGB(r,g,b);
		}else{
	#endif
			return LEDs[0].Color(r,g,b,w);
	#ifdef FASTLED
		}	
	#endif
}
uint32_t getLedColorGamma(uint8_t r, uint8_t g, uint8_t b, uint8_t w){
	return LEDs[0].gamma32(getLedColor(r,g,b,w));
}
uint32_t getLedColorHSV(uint16_t hue, uint8_t sat, uint8_t val){
	return LEDs[0].gamma32(LEDs[0].ColorHSV(hue,sat,val));
}


void ledBrightness(uint8_t brightness){
	for (uint8_t stripNumber = 0; stripNumber < ledStripCount; stripNumber++){
		#ifdef FASTLED
			if(ledLibrary==FASTLED_LIBRARY){
				FastLED.setBrightness(brightness);
			}else{
		#endif
				LEDs[stripNumber].setBrightness(brightness);
		#ifdef FASTLED
			}		
		#endif
	}
}

void show(){
	for (uint8_t stripNumber = 0; stripNumber < ledStripCount; stripNumber++){
	#ifdef FASTLED
		if(ledLibrary==FASTLED_LIBRARY){
			FastLED.show();
		}else{
	#endif
		LEDs[stripNumber].show();
	#ifdef FASTLED
		}		
	#endif
	}

}

uint16_t getHue(uint8_t red, uint8_t green, uint8_t blue) {
    uint8_t min = min(min(red, green), blue);
    uint8_t max = max(max(red, green), blue);

	uint8_t value = max;
	uint16_t hue = 0;
	uint8_t sat = 0;
	if(value == 0){
		hue = 0;
		sat = 0;
		return 0;
	}
    if (min == max) {
        return 0;
    }

    sat = 255 * long(max - min) / value;
    if (sat == 0){
        hue = 0;
        return 0;
    }

    if (max == red){
        hue = 0 + 43 * (green - blue) / (max - min);
	}else if (max == green){
        hue = 85 + 43 * (blue - red) / (max - min);
	}else{
        hue = 171 + 43 * (red - green) / (max - min);
	}
	if(hue > 500){
		hue +=256;
	}
	hue *= 256;
    return hue;

}

/*
HsvColor RgbToHsv(RgbColor rgb)
{
    HsvColor hsv;
    unsigned char rgbMin, rgbMax;

    rgbMin = rgb.r < rgb.g ? (rgb.r < rgb.b ? rgb.r : rgb.b) : (rgb.g < rgb.b ? rgb.g : rgb.b);
    rgbMax = rgb.r > rgb.g ? (rgb.r > rgb.b ? rgb.r : rgb.b) : (rgb.g > rgb.b ? rgb.g : rgb.b);

    hsv.v = rgbMax;
    if (hsv.v == 0)
    {
        hsv.h = 0;
        hsv.s = 0;
        return hsv;
    }

    hsv.s = 255 * long(rgbMax - rgbMin) / hsv.v;
    if (hsv.s == 0)
    {
        hsv.h = 0;
        return hsv;
    }

    if (rgbMax == rgb.r)
        hsv.h = 0 + 43 * (rgb.g - rgb.b) / (rgbMax - rgbMin);
    else if (rgbMax == rgb.g)
        hsv.h = 85 + 43 * (rgb.b - rgb.r) / (rgbMax - rgbMin);
    else
        hsv.h = 171 + 43 * (rgb.r - rgb.g) / (rgbMax - rgbMin);

    return hsv;
}
*/

void pixelhue(uint8_t num, uint16_t huein,uint16_t offset){
	for (uint8_t stripNumber = 0; stripNumber < ledStripCount; stripNumber++){
		#ifdef FASTLED
		if(ledLibrary==FASTLED_LIBRARY){
		}else{
		#endif
			LEDs[stripNumber].setPixelColor(num,LEDs[stripNumber].gamma32(LEDs[0].ColorHSV(huein+offset*100,255,255)));
		#ifdef FASTLED
		}	
		#endif
	}
}


void pixelhueStrip(uint8_t strip,uint8_t num, uint16_t huein,uint16_t offset){
	#ifdef FASTLED
	if(ledLibrary==FASTLED_LIBRARY){
	}else{
	#endif
		LEDs[strip].setPixelColor(num,LEDs[strip].gamma32(LEDs[strip].ColorHSV(huein+offset*100,255,255)));
	#ifdef FASTLED
	}
	#endif
}

void pixelhueStripValue(uint8_t strip,uint8_t num, uint16_t huein,uint16_t offset,int white){
	#ifdef FASTLED
	if(ledLibrary==FASTLED_LIBRARY){
	}else{
	#endif
		if(white<0){
			LEDs[strip].setPixelColor(num,LEDs[strip].gamma32(LEDs[strip].ColorHSV(huein+offset*100,255,white+255)));
		}else{
			LEDs[strip].setPixelColor(num,LEDs[strip].gamma32(LEDs[strip].ColorHSV(huein+offset*100,255,255)+(white*16777216)));
		}
	#ifdef FASTLED
	}	
	#endif
}

void pixelc(uint8_t num, uint8_t red, uint8_t green, uint8_t blue, uint8_t white){
	#ifdef FASTLED
		if(ledLibrary==FASTLED_LIBRARY){
			leds[0][num] = CRGB(red,green,blue);
		}else{
	#endif
		LEDs[0].setPixelColor(num,LEDs[0].Color(red,green,blue,white));
	#ifdef FASTLED
		}	
	#endif
}
uint32_t gamma(uint32_t colorIn){
	return LEDs[0].gamma32(colorIn);
}

uint32_t getPixelc(uint16_t num){
	return LEDs[0].getPixelColor(num);
}

void pixelcol(uint8_t strip,uint8_t num, uint32_t color){
    #ifdef FASTLED
		if(ledLibrary==FASTLED_LIBRARY){
			leds[0][num] = color;
		}else{
	#endif			
		LEDs[strip].setPixelColor(num,color);
		switch (layout){
		case layoutDouble:
			LEDs[strip].setPixelColor(num+NumberLeds,color);
			break;
		case layoutReverse:
			LEDs[strip].setPixelColor((NumberLeds<<1)-1-num,color);
			break;
		
		default:
			break;
		}

	#ifdef FASTLED
    	}	
	#endif
}
// "Popular" Colors
uint32_t ledBlack(){
	#ifdef FASTLED
		if(ledLibrary==FASTLED_LIBRARY){
			return CRGB::Black;
		}else{
		#endif
			return LEDs[0].Color(0,0,0,0);
		#ifdef FASTLED
			}			
		#endif
}
uint32_t ledRed(){
	#ifdef FASTLED
		if(ledLibrary==FASTLED_LIBRARY){
			return CRGB::Red;
		}else{
	#endif
		return LEDs[0].Color(255,0,0,0);
	#ifdef FASTLED
		}			
	#endif
}
uint32_t ledBlue(){
	#ifdef FASTLED
		if(ledLibrary==FASTLED_LIBRARY){
			return CRGB::Blue;
		}else{
	#endif
		return LEDs[0].Color(0,0,255,0);
	#ifdef FASTLED
		}			
	#endif
}
uint32_t ledYellow(){
	#ifdef FASTLED
		if(ledLibrary==FASTLED_LIBRARY){
			return CRGB::Yellow;
		}else{
	#endif
		return LEDs[0].Color(255,255,0,0);
	#ifdef FASTLED
		}			
	#endif
}
uint32_t ledGreen(){
	#ifdef FASTLED
		if(ledLibrary==FASTLED_LIBRARY){
			return CRGB::Green;
		}else{
	#endif
		return LEDs[0].Color(0,255,0,0);
	#ifdef FASTLED
		}			
	#endif
}

// library independent
void pixelcol1(uint8_t num, uint32_t color){
		pixelcol(0,num,color);
}
void pixelcol2(uint8_t num, uint32_t color){
		pixelcol(1,num,color);
}
void setBar(uint8_t number, uint32_t color){
	for (uint8_t stripNumber = 0; stripNumber < ledStripCount; stripNumber++){
		pixelcol(stripNumber,number,color);
	}
}


void allOff(){
	for (uint8_t stripNumber = 0; stripNumber < ledStripCount; stripNumber++){
		for(uint8_t number = 0; number < NumberLeds; number++) {
			pixelcol(stripNumber,number,ledBlack());
		}
	}
}
