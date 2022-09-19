#include <Arduino.h>
#include "constants.h"
#include "debug.h"

// Control which messages to see
#define GENERAL_ACTIVE       true
#define CONFIGURATION_ACTIVE true
#define TOUCH_ACTIVE         false
#define LED_ACTIVE           true
#define PATTERN_ACTIVE       false


void setupDebug(){
    // Usually the main project does this for you
    //    Serial.begin(57600);
}
void showDebugOn(){
	// Show which debug is on
    #ifdef LOWMEM
	debugn('G',"D Gen");
	debugn('C',"D Conf");
	debugn('T',"D Touch");
	debugn('L',"D LED");
    #else
	debugn('G',"Debug General Active");
	debugn('C',"Debug Configuration Active");
	debugn('T',"Debug Touch Active");
	debugn('L',"Debug LED Active");
    #endif
}

bool isActive(char typ){
    switch (typ)
    {
    case 'G': // General
        return GENERAL_ACTIVE;
    case 'C': // Configuration
        return CONFIGURATION_ACTIVE;
    case 'T': // Touch
        return TOUCH_ACTIVE;
    case 'L': // LED
        return LED_ACTIVE;
    case 'P': // PATTERN
        return PATTERN_ACTIVE;
    default:
        return true;
    }
    // G = General
    // C = Configuration
    // L = LED
}

void debugn(char typ){
    if(isActive(typ)){
        Serial.println();
    }
}

void debug(char typ, const String &s){
    if(isActive(typ)){
        Serial.print(s);
    }
}
void debugn(char typ, const String &s){
//    Serial.println("...");
    if(isActive(typ)){
        Serial.println(s);
    }
//        Serial.println(s);
//    Serial.println("---");
}

void debugI(unsigned char b, int base){
    Serial.print(b,base);
}

void debugI(char c){
    Serial.print(c);
}
void debugIn(char c){
    Serial.println(c);
}

void debugI(uint16_t ui){
    Serial.print(ui);
}
void debugIn(uint16_t ui){
    Serial.println(ui);
}
void debugI(uint8_t ui){
    Serial.print(ui);
}
void debugIn(uint8_t ui){
    Serial.println(ui);
}
void debug(char typ, int ui){
    if(isActive(typ)){
      Serial.print(ui);
    }
}
void debugn(char typ, int ui){
    if(isActive(typ)){
        Serial.println(ui);
    }
}
void debugI(unsigned long ul){
    Serial.print(ul);
}
void debugIn(unsigned long ul){
    Serial.println(ul);
}
void debugI(float f){
    Serial.print(f);
}
void debugIn(float f){
    Serial.println(f);
}
