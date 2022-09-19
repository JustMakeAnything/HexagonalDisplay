#include <Arduino.h>
#include "constants.h"
#include "configuration.h"
#include "patterns.h"
#include "state.h"

#ifdef DEBUG
#include <debug.h>
#endif


uint8_t patternNumber;
uint8_t brightLevel = 1;

void setupState(){
    patternNumber = getFavourite();
}

void setPatternTo(uint8_t pattern){
    patternNumber = pattern;
}

void nextPattern(boolean back){
    uint8_t patternCount = getPatternCount();
  	patternNumber++; 
    if(back){
        patternNumber -= 2; 
    }
    patternNumber %= patternCount;
    while(getPatternConfig(patternNumber)<0){
      	patternNumber += 1; 
        patternNumber %= patternCount;
    }
    resetPattern();
    #ifdef DEBUG
	debug('T',patternNumber);    
    #endif
}

void setPatternFadeOff(){
    if(patternNumber == 99){
        patternNumber = 98;    
    }else{
        patternNumber = 99;
    }
}

uint8_t getPatternNumber(){
    if(patternNumber>90){
        return patternNumber;
    }else{
        // Pattern is stored +1 to distinguish +1/-1 (not possible for 0)
        return getPatternConfig(patternNumber)-1;
    }
}

void nextBrightLevel(){
  	brightLevel += 1;
    brightLevel %= 3;
}

uint8_t getBrightLevel(){
    return brightLevel;
}
