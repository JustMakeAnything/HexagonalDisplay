#include <Arduino.h>
#include "fastMath.h"
#include "fastled.h"

uint16_t calculateDistance(int16_t physX,int16_t physY,int16_t  posX,int16_t posY){
    // Not suitable for 1 dimensional, but usable for 2D
    // Distance between phys and pos
    uint32_t squared = (abs(physX - posX)>>8) * (abs(physX - posX)>>8) +
                       (abs(physY - posY)>>8) * (abs(physY - posY)>>8);
    if(squared>65535){
        return 65535;
    }
    return (uint16_t)sqrt16(squared)<<8;
}