#include <Arduino.h>

void setupDebug();
void showDebugOn();
void debugn(char typ);

void debug(char typ, const String &s);
void debugn(char typ, const String &s);

void debugI(unsigned char b, int base);
void debugIn(unsigned char b, int base);

void debugI(char c);
void debugIn(char c);

void debugI(uint16_t ui);
void debugIn(uint16_t ui);

void debugI(uint8_t ui);
void debugIn(uint8_t ui);

void debug(char typ, int ui);
void debugn(char typ, int ui);

void debugI(unsigned long ul);
void debugIn(unsigned long ul);

void debugI(float f);
void debugIn(float f);
