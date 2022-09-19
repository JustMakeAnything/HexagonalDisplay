#include <Arduino.h>
#include "constants.h"
#include "configuration.h"
#include "EEPROM.h"
#include "touch.h"
#include "fastMath.h"
#include "fastled.h"
#include "ledStrip.h"
#include "patterns.h"
#include "actions.h"
#ifdef DEBUG
#include "debug.h"
#endif

// 168 has 512 bytes, 328:1024
#define CONF_LED_LIBRARY 1
#define CONF_STRIP_TYPE 2
#define CONF_RGB_ORDER 3
#define CONF_HAS_WHITE 4
#define CONF_NUM_LEDS 5
#define CONF_NUM_STRIPS 6
#define CONF_NUM_SENSORS 7
#define CONF_FAVOURITE 8
#define CONF_LED_LAYOUT 9
#define CONF_TRIGGERSENSOR 10
#define CONF_SENSOR_TRESHOLD 20   // 20-25 (A0-A5)
#define CONF_SENSORS_FUNCTION 40 // 40-45 Sensor 1 to 6
#define CONF_PATTERNS         50 // 50-99 Patterns up to MAXPATTERNS

struct config_t{
    uint8_t ledLibrary; // Neopixels only
    uint8_t stripType;   //RGBW or RGB      Work
    uint8_t rbgOrder;    //R,G,B sequence    in 
    bool hasWhite;       //....             Progress
    uint8_t numberOfLeds;
    uint8_t numberOfStrips;
    uint8_t numberOfSensors;
    uint8_t sensorTreshold[MAX_SENSORS];
    uint8_t sensorFunction[MAX_SENSORS];
    int8_t  patterns[MAXPATTERNS];
    uint8_t favourite;
    uint8_t triggerSensor; //To trigger special effects
};

config_t configData;

void saveStripData();
void writeDefaultConfig();
void readPatternData();


void setupConfiguration(){
    EEPROM.begin();
    EEPROM.write(CONF_LED_LAYOUT,layoutSingle);

    if(EEPROM.read(0) != MAGICCODE){
        writeDefaultConfig();
    }; 
    configData.ledLibrary       = EEPROM.read(CONF_LED_LIBRARY);
    configData.stripType        = EEPROM.read(CONF_STRIP_TYPE);
    configData.rbgOrder         = EEPROM.read(CONF_RGB_ORDER);
    configData.hasWhite         = EEPROM.read(CONF_HAS_WHITE);
    configData.numberOfLeds     = EEPROM.read(CONF_NUM_LEDS);
    configData.numberOfStrips   = EEPROM.read(CONF_NUM_STRIPS);
    configData.numberOfSensors  = EEPROM.read(CONF_NUM_SENSORS);
    configData.favourite        = EEPROM.read(CONF_FAVOURITE);
    configData.favourite        = EEPROM.read(CONF_TRIGGERSENSOR);    
    // Read sensor tresholds and functions
    for (byte sensorNo = 0; sensorNo < MAX_SENSORS; sensorNo++){
        configData.sensorTreshold[sensorNo] = EEPROM.read(CONF_SENSOR_TRESHOLD+sensorNo);
        configData.sensorFunction[sensorNo] = EEPROM.read(CONF_SENSORS_FUNCTION+sensorNo);
    }
    readPatternData();
}

void writeDefaultConfig(){
        EEPROM.write(0,MAGICCODE);
        // Initial values
        configData.ledLibrary = NEOPIXEL_LIBRARY;
        configData.stripType = STRIP_TYPE_WS2811;
        configData.rbgOrder = ORDER_GRB; //ORDER_RGB;
        configData.hasWhite = false;
        configData.numberOfLeds = DEFAULT_NUM_LEDS;
        configData.numberOfStrips = 1;
        configData.numberOfSensors = MAX_SENSORS;
        configData.favourite = 1;
        for (uint8_t sensorNo = 0; sensorNo < MAX_SENSORS; sensorNo++){
            configData.sensorTreshold[sensorNo] = 10;
            configData.sensorFunction[sensorNo] = sensorNo;
        }
        for (uint8_t patternNo = 0; patternNo < MAXPATTERNS; patternNo++){
            configData.patterns[patternNo] = patternNo+1;
        }
        saveStripData();
	    #ifdef DEBUG
            #ifdef LOWMEM
            debugn('C',"init EEPROM");
            #else
            debugn('C',"Initial EEPROM values written");
            #endif
        #endif
}

void readPatternData(){
    uint8_t patternCount = getPatternCount();
    bool overflown = false;
    for (byte patternNo = 0; patternNo < patternCount; patternNo++){
        uint8_t patternIndex = EEPROM.read(CONF_PATTERNS+patternNo);
        configData.patterns[patternNo] = patternIndex;
        // Check for overflows, then reset the configuration in memory
        // it will be saved to EEPROM when config is entered manually
        if(patternIndex>patternCount || patternIndex == 0){
            overflown = true;
        }
    }
    if(overflown==true){
        // Config is reset here in memory, not in EEPROM
        // All elements are reset, also the not possible (MAXPATTERNS)
        for (uint8_t patternNo = 0; patternNo < MAXPATTERNS; patternNo++){
            configData.patterns[patternNo] = (patternNo+1);
        }
        // Also reset favourite since it could be out of bounds
        configData.favourite = 1;
    }
}

void saveStripData(){
    EEPROM.write(CONF_LED_LIBRARY,configData.ledLibrary);
    EEPROM.write(CONF_STRIP_TYPE,configData.stripType);
    EEPROM.write(CONF_RGB_ORDER,configData.rbgOrder);
    EEPROM.write(CONF_HAS_WHITE,configData.hasWhite);
    EEPROM.write(CONF_NUM_LEDS,configData.numberOfLeds);
    EEPROM.write(CONF_NUM_STRIPS,configData.numberOfStrips);
    EEPROM.write(CONF_NUM_SENSORS,configData.numberOfSensors);
    EEPROM.write(CONF_FAVOURITE,configData.favourite);
    EEPROM.write(CONF_TRIGGERSENSOR,configData.triggerSensor);
    for (byte sensorNo = 0; sensorNo < MAX_SENSORS; sensorNo++){
        EEPROM.write(CONF_SENSOR_TRESHOLD+sensorNo,configData.sensorTreshold[sensorNo]);
        EEPROM.write(CONF_SENSORS_FUNCTION+sensorNo,configData.sensorFunction[sensorNo]);
    }
    for (byte patternNo = 0; patternNo < MAXPATTERNS; patternNo++){
        EEPROM.write(CONF_PATTERNS+patternNo,configData.patterns[patternNo]);
    }
}

boolean isMenuActive(){
    return true;
}

uint8_t getConfigLedLibrary(){
    return EEPROM.read(CONF_LED_LIBRARY);
}

uint8_t getLedStripCount(){
    return 1;
    // Also change MAX_NUM_LEDS
    uint8_t stripCount = EEPROM.read(CONF_NUM_STRIPS);
    stripCount = min(stripCount,MAX_LED_STRIP_COUNT);
    return stripCount;
}

uint8_t getLedLayout(){
    return layoutSingle;
    return EEPROM.read(CONF_LED_LAYOUT);
}
uint8_t getLedLayoutFactor(){
    // The times the pattern is repeated
    switch (getLedLayout()){
    case layoutDouble:
    case layoutReverse:
        return 2;
    case layoutQuad:
        return 4;
    }
    return 1;
}

uint8_t getConfigDataPin(uint8_t stripNumber){
    if(stripNumber<1 || stripNumber > MAX_LED_STRIP_COUNT){
	    #ifdef DEBUG
        #ifdef LOWMEM
        debug('C',"E Strip : ");
        #else
        debug('C',"Error: Invalid Strip Number: ");
        #endif
        debugn('C',stripNumber);
        #endif
        return 0;
    }
    return EEPROM.read(10+stripNumber);
}

uint16_t getAmountOfLeds(){
    // All strips have the same size
    configData.numberOfLeds = EEPROM.read(CONF_NUM_LEDS);
    #ifdef DEBUG
    debugn('C',configData.numberOfLeds);
    #endif
    return min(MAX_NUM_LEDS,configData.numberOfLeds);
}

void setNumberOfLeds(int amount){
    configData.numberOfLeds = amount;
    EEPROM.write(CONF_NUM_LEDS,amount);
}

void setNumberOfSensors(int amount){
    configData.numberOfSensors = amount;
    EEPROM.write(CONF_NUM_SENSORS,amount);
}
uint8_t getNumberOfSensors(){
    return configData.numberOfSensors;
}
uint8_t getSensorValue(uint8_t sensorNo){
    return sqrt16(getTouchRaw(sensorNo));

}
uint8_t getSensorTreshold(uint8_t sensorNo){
    return configData.sensorTreshold[sensorNo];
}
uint8_t getTriggerSensorValue(){
    // You can assign a touch function to a sensor. 
    // It returns a value between 0 and 255 where 255 is the value at the treshold
    // everything above the threshold is set to 255
    int value = getSensorValue(configData.triggerSensor) * 255 / getSensorTreshold(configData.triggerSensor);
    return min(255,value);
}
uint8_t getFunctionForSensor(uint8_t sensorNo){
    return configData.sensorFunction[sensorNo];
}
void setFunctionForSensor(uint8_t sensorNo,uint8_t sensorFunction){
    configData.sensorFunction[sensorNo] = sensorFunction;
    EEPROM.write(CONF_SENSORS_FUNCTION+sensorNo,configData.sensorFunction[sensorNo]);
    // Special for trigger since it is stored separately
    if(sensorFunction==FunctionTriggerSensor){
        configData.triggerSensor = sensorNo;
        EEPROM.write(CONF_TRIGGERSENSOR,configData.triggerSensor);
    }
}
uint8_t getFavourite(){
    return configData.favourite;
}
uint8_t getTriggerSensor(){
    return configData.triggerSensor;
}
uint8_t showHint=0;
void calibrateSensors(){

    // Also show on LEDstrip
    allOff();
    setBrightness(2);

    #define BARSIZE 21
    boolean finished=false;
    byte selected = 0;
    do{
        if(Serial.available()>0){
            char input = Serial.read();
            switch (input){
            case 'q':
                if(selected>0){selected--;};
                break;
            case 'w':
                selected++;
                if(selected>=configData.numberOfSensors){selected=configData.numberOfSensors-1;};
                break;
            case 'o':
                if(configData.sensorTreshold[selected]>0){
                    configData.sensorTreshold[selected]--;
                }
                break;
            case 'p':
                if(configData.sensorTreshold[selected]<=BARSIZE){
                    configData.sensorTreshold[selected]++;
                }
                break;
            case '.':
                finished = true;
                saveStripData();
                break;
            
            default:
                showHint = 20;
                break;
            }
        }
        readTouch(); // Only one sensor is read per call
        readTouch(); // read two to avoid lags
        Serial.print("|");
        for (byte sensorIndex = 0; sensorIndex < configData.numberOfSensors; sensorIndex++){
            uint8_t barValue = sqrt16(getTouchRaw(sensorIndex));
            for (byte barIndex = 0; barIndex < BARSIZE; barIndex++){

                if(barIndex==getSensorTreshold(sensorIndex)){
                        if(barIndex>=barValue){
                            Serial.print("o");
                        }else{
                            Serial.print("O");
                        }
                }else{
                    if(barIndex<barValue){
                        if(selected==sensorIndex){
                            Serial.print("X");
                        }else{
                            Serial.print("x");
                        }
                    }else{
                        if(selected==sensorIndex){
                            Serial.print("-");
                        }else{
                            Serial.print(" ");
                        }
                    }
                }
            }
            Serial.print("|");
            // Control LED Strip
            // It lights up according to the values
            // When the treshold is reached it gets much brighter
            uint8_t bright = barValue * 5 ;
            if(barValue>=getSensorTreshold(sensorIndex)){
                bright = 100 + barValue * 5;
            }
            setBar(sensorIndex,getLedColor(0,bright,bright,0));
        }
        show();
        if(showHint>0){
            Serial.print(" q/w=sens o/p=thresh .=back");
            showHint--;
        }
        Serial.println();
        delay(50);
    } while (finished == false);
    
    // Own loop
}

void showPatternState(uint8_t selected){
// Show current state
    for (uint8_t line = 0; line < 2; line++){
        for (uint8_t patternIndex = 0; patternIndex < getPatternCount(); patternIndex++){
            int8_t pattern = configData.patterns[patternIndex];

            // Before
            if(patternIndex==selected){
                Serial.print(">");
            }else{
                Serial.print(" ");
            }

            // Leading space when >9
            #if MAXPATTERNS > 9
            if(pattern<10){
                Serial.print(" ");
            }
            #endif
            if(line == 0){
                if(pattern>0){
                    Serial.print(pattern);
                }else{
                    Serial.print(" ");
                }
            }else{
                if(pattern<0){
                    Serial.print(-pattern);
                }else if(configData.favourite==patternIndex){
                    Serial.print("F");
                }else{
                    Serial.print(" ");
                }
            }
            if(patternIndex==selected){
                Serial.print("<");
            }else{
                Serial.print(" ");
            }

        }
        Serial.println();
    }
    Serial.println(" q/w=patt o/p=move f=fav space=on/off .=back");
}

void swapWithNext(int8_t selected){
    int8_t temp = configData.patterns[selected];
    configData.patterns[selected] = configData.patterns[selected+1];
    configData.patterns[selected+1] = temp;
}

int8_t getPatternConfig(uint8_t patternIndex){
    uint8_t patternNumber = configData.patterns[patternIndex];
    // // Safety feature since otherwise it may crash when a pattern was removed
    // patternNumber %= getPatternCount();
    return patternNumber;
}

void configurePatterns(){

    boolean finished=false;
    uint8_t selected = 0;
    uint8_t patternCount = getPatternCount();
    showPatternState(selected);
    do{
        if(Serial.available()>0){
            char input = Serial.read();

            switch (input){
            case 32: // Toggle on/off
                if(getFavourite()!=selected){
                    configData.patterns[selected] = -configData.patterns[selected];
                }
                break;
            case 'q':
                if(selected>0){selected--;};
                resetPattern();
                break;
            case 'w':
                if(selected<patternCount-1){selected++;};
                resetPattern();
                break;
            case 'o':
                if(selected>0){
                    selected--;
                    swapWithNext(selected);
                };
                break;
            case 'p':
                if(selected<patternCount-1){
                    swapWithNext(selected);
                    selected++;
                }
                break;
            case 'f':
                if(configData.patterns[selected]>=0){
                    configData.favourite = selected;
                }
                break;
            case '.':
                finished = true;
                saveStripData();
                break;
            
            default:
                break;
            }
            showPatternState(selected);
        }
		displayPattern(abs(configData.patterns[selected])-1);
		delay(50);
    } while (finished == false);
}

#ifdef MENU_LED_ORDER
void switchLedOrderRed(){
    uint8_t pType = EEPROM.read(CONF_RGB_ORDER);
    switch(pType){
        case 1: pType = 2; break;
        case 2: pType = 3; break;
        case 3: pType = 1; break;
        case 4: pType = 6; break;
        case 5: pType = 4; break;
        case 6: pType = 5; break;
    #ifdef DEBUG
    default:
        debug('C',"Invalid pixeltype: ");
        debugn('C',pType);
    #endif
    }
    EEPROM.write(CONF_RGB_ORDER,pType);
}
void switchLedOrderGreen(){
    uint8_t pType = EEPROM.read(CONF_RGB_ORDER);
    switch(pType){
        case 1: pType = 6; break;
        case 2: pType = 1; break;
        case 3: pType = 4; break;
        case 4: pType = 5; break;
        case 5: pType = 3; break;
        case 6: pType = 2; break;
    #ifdef DEBUG
    default:
        debug('C',"Invalid pixeltype: ");
        debugn('C',pType);
    #endif
    }
    #ifdef DEBUG
        debugn('C',pType);
    #endif
    EEPROM.write(CONF_RGB_ORDER,pType);
}
void switchLedOrderBlue(){
    uint8_t pType = EEPROM.read(CONF_RGB_ORDER);
    switch(pType){
        case 1: pType = 5; break;
        case 2: pType = 4; break;
        case 3: pType = 2; break;
        case 4: pType = 3; break;
        case 5: pType = 6; break;
        case 6: pType = 1; break;
    #ifdef DEBUG
    default:
        debug('C',"Invalid pixeltype: ");
        debugn('C',pType);
    #endif
    }
    #ifdef DEBUG
        debugn('C',pType);
    #endif
    EEPROM.write(CONF_RGB_ORDER,pType);
}
#endif

#ifdef FASTLED
void switchLedLibrary(){
    uint8_t library = EEPROM.read(CONF_LED_LIBRARY);
    library++;
    library %=2; // FASTLED_LIBRARY, NEOPIXEL_LIBRARY
    EEPROM.write(CONF_LED_LIBRARY, library);
}
#endif


uint8_t getPixelOrder(){
    return EEPROM.read(CONF_RGB_ORDER);
}

neoPixelType getPixelType(){
    neoPixelType NPType;
    switch (EEPROM.read(CONF_HAS_WHITE)){
        case 0:
            NPType = getPixelOrder();
            NPType = NEO_GRB;
            break;
        
        default:
            NPType = getPixelOrder()|192;
//            NPType = NEO_GRBW;
            break;
    }

    NPType += NEO_KHZ800;
    return NPType;
//   return NEO_GRBW + NEO_KHZ800;
} 
    
