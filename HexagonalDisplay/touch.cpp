#include <Arduino.h>
#include "constants.h"
#define analogSensor 1
#ifdef DEBUG
#include <debug.h>
#endif
#include "state.h"
#ifdef analogSensor
    #include <ADCTouch.h>
#else
    #include <CapacitiveSensor.h>
#endif
#include "touch.h"
#include "configuration.h"



#ifdef analogSensor
    int touchRaw[MAX_SENSORS];
    int touchRef[MAX_SENSORS];
    bool triggered[MAX_SENSORS];
    uint8_t sensorRotation=0; // Only one sensor is read at a time to avoid stuttering
#else
    CapacitiveSensor   cs_1  = CapacitiveSensor(TOUCH_COMMON,TOUCH_ONOFF);        // 10M resistor between pins 2 and sensor
    CapacitiveSensor   cs_2  = CapacitiveSensor(TOUCH_COMMON,TOUCH_2);    
    CapacitiveSensor   cs_3 = CapacitiveSensor(TOUCH_COMMON,TOUCH_3);    
#endif

void setupTouch(){
#ifdef analogSensor
    for (byte sensorNo = 0; sensorNo < MAX_SENSORS; sensorNo++){
        touchRef[sensorNo] = ADCTouch.read(A0+sensorNo, 500); 
    }
#else
    cs_1.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
    cs_2.set_CS_AutocaL_Millis(0xFFFFFFFF);
    cs_3.set_CS_AutocaL_Millis(0xFFFFFFFF);
#endif
}

boolean trigger(bool &old){
    if(old){
         return false;
    }
    old = true;
    return true;
}

int getTouchRaw(uint8_t sensorNo){
    if(touchRaw[sensorNo]<0){
        return 0;
    }
    return touchRaw[sensorNo];
}

bool isAboveTreshold(uint8_t sensorNo){
    if(getSensorValue(sensorNo)>=getSensorTreshold(sensorNo)){
        return true;
    }else{
        return false;
    }
}

bool isTriggered(uint8_t sensorNo){
    if(isAboveTreshold(sensorNo)){
        bool before = triggered[sensorNo];
        triggered[sensorNo] = true;
        return trigger(before);
    }else{
        triggered[sensorNo] = false;
        return false;
    }
}

void readTouch(){
    sensorRotation++;
    sensorRotation %= getNumberOfSensors();
    touchRaw[sensorRotation] = ADCTouch.read(A0+sensorRotation)-touchRef[sensorRotation];
}
