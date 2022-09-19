#include <Arduino.h>
#include "constants.h"
#include "actions.h"
#include "state.h"
#include "configuration.h"
#include "touch.h"


void executeActions(){
    for (uint8_t sensorNo = 0; sensorNo < MAX_SENSORS; sensorNo++){
        if(isTriggered(sensorNo)){
            switch (getFunctionForSensor(sensorNo)){
            case FunctionSwitchBrightness:
                nextBrightLevel();
                break;
            case FunctionNextPattern:
                nextPattern(false);
                break;
            case FunctionPrevPattern:
                nextPattern(true);
                break;
            case FunctionFavouritePattern:
                setPatternTo(getFavourite());
                break;
            /* Actively read
            case FunctionTriggerSensor:
                setPatternTo(getFavourite());
                break;
                */
            default:
                break;
            }
        }
    }
}
