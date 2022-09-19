#include <Arduino.h>

#define sgn(x) ((x) < 0 ? -1 : ((x) > 0 ? 1 : 0))
uint16_t calculateDistance(int16_t physX,int16_t physY,int16_t  posX,int16_t posY);