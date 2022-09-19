#include <Arduino.h>
#include "constants.h"
#ifdef MENU
#include "configuration.h"
#include "CustomSerialMenu.h"
#include "patterns.h"
#include "ledStrip.h"
#include "touch.h"
#include "state.h"
#include "actions.h"


SerialMenu& myMenu = SerialMenu::get();

extern const SerialMenuEntry mainMenu[];
extern const SerialMenuEntry functionsMenu[];
extern const SerialMenuEntry functionsDetailMenu[];

#ifdef MENU_LED_STRIP
extern const SerialMenuEntry ledMenu[];
extern const uint8_t ledMenuSize;
const char menuTLEDStrip[]       PROGMEM = "[L]ed Strip";
#endif
const char menuTAmount[]         PROGMEM = "[A]mount";
const char menuTSensor[]         PROGMEM = "[S]ensors";
const char menuTPattern[]        PROGMEM = "[P]atterns";
const char menuTBackToPrevMenu[] PROGMEM = "[.]Back";
const char menuTShowMenu[]       PROGMEM = "show [M]enu";

extern const SerialMenuEntry mainMenu[];
extern const uint8_t mainMenuSize;
extern const SerialMenuEntry sensorMenu[];
extern const uint8_t sensorMenuSize;
extern const SerialMenuEntry patternMenu[];
extern const uint8_t patternMenuSize;

uint8_t currentSensorNo = 1;
char functionText[5][10];

void showMain(){
  myMenu.load(mainMenu, mainMenuSize);
//  setPatternTo(1);
//  Serial.read();
  myMenu.show();
}

// Main menu
const SerialMenuEntry mainMenu[] = {
  {
    #ifdef MENU_LED_STRIP    
    menuTLEDStrip, true,'l',[](){ myMenu.load(ledMenu, ledMenuSize);
//                                    				setPatternTo(102);
                                            myMenu.show(); }
  },{
    #endif
    menuTSensor, true,'s',[](){ myMenu.load(sensorMenu, sensorMenuSize);
//                                    				setPatternTo(1);
                                            myMenu.show(); }
  },{
    #ifdef MENU_PATTERNS
    menuTPattern, true,'p',[](){ configurePatterns();myMenu.show();/*myMenu.load(patternMenu, patternMenuSize);
                                    				setPatternTo(3);  // Test only
                                            myMenu.show(); */}
  },{
    #endif
    menuTShowMenu,true,'m',[](){ showMain(); }
  }
};
constexpr uint8_t mainMenuSize = GET_MENU_SIZE(mainMenu);

void changeLedOrderRed(){
  switchLedOrderRed();
  setupLeds();
};
void toggleWhite(){};
void toggleProtocol(){};
void amountOfLeds(){
  uint8_t amount = myMenu.getNumber<uint8_t>("Amount:");
  if (amount < 1){
    amount = 1;
  } 
  setNumberOfLeds(amount);
};
void amountOfSensors(){
  uint8_t amount = myMenu.getNumber<int>("Amount:");
  if (amount < 1){
    amount = 1;
  } 
  if (amount > 6){
    amount = 6;
  } 
  setNumberOfSensors(amount);
};
// LED Menu
#ifdef MENU_LED_STRIP
const char menuTVerifyRGBW[] PROGMEM = "[C]heck RGBW";
const SerialMenuEntry ledMenu[] = {
{
  /*
  #ifdef MENU_LED_ORDER
    "Reorder [R]ed", false,'r',[](){ switchLedOrderRed();setupLeds(); }
  },{
    "Reorder [G]reen", false,'g',[](){ switchLedOrderGreen();setupLeds(); }
  },{
    "Reorder [B]lue", false,'b',[](){ switchLedOrderBlue();setupLeds(); }
  },{
  #endif
  */
/*
    #ifdef FASTLED    
    "[N]o White",    false,'n',[](){ toggleWhite(); }
  },{
    "[S]trange colors", false,'s',[](){ toggleProtocol(); }
  },{
    #endif
    */
    menuTVerifyRGBW, true,'c',[](){ setPatternTo(101); }
  },{
    menuTAmount, true,'a',[](){ allOff();show();amountOfLeds();/*setPatternTo(102);*/setupPatterns();}
  },{
    menuTShowMenu,true,'m',[](){ myMenu.show(); }
  },{
    menuTBackToPrevMenu, true,'.',[](){ showMain(); }
  }
};
constexpr uint8_t ledMenuSize = GET_MENU_SIZE(ledMenu);
#endif

void setSensorNo(uint8_t number){
  currentSensorNo = number;
}
void setFunction(uint8_t functionNo){
  setFunctionForSensor(currentSensorNo,functionNo);
}

void setChoosenFunctionTexts();
void toFunctionsMenu();

void toSensorDetailMenu();
#ifdef MENU_FUNCTIONS
//void updateSensorDetailMenu(uint8_t sensorFunction);
void updateSensorDetailMenu(uint8_t sensorFunction){
  setFunction(sensorFunction);
  toSensorDetailMenu();
}
//const char menuTFunction1[] PROGMEM = "[O]n/off";
const char menuTFunction2[] PROGMEM = "[N]ext P";
const char menuTFunction3[] PROGMEM = "[P]rev P";
const char menuTFunction4[] PROGMEM = "[B]right";
const char menuTFunction5[] PROGMEM = "[F]avour";
const char menuTFunction6[] PROGMEM = "[T]rigg.";
const SerialMenuEntry functionsDetailMenu[] = {
  // The function enum has to be in the correct order for the mark to be shown correctly
  {
//    menuTFunction1, true,'o',[](){ updateSensorDetailMenu(FunctionOnOff); }
//  },{
    menuTFunction2, true,'n',[](){ updateSensorDetailMenu(FunctionNextPattern); }
  },{
    menuTFunction3, true,'p',[](){ updateSensorDetailMenu(FunctionPrevPattern); }
  },{
    menuTFunction4, true,'b',[](){ updateSensorDetailMenu(FunctionSwitchBrightness); }
  },{
    menuTFunction5, true,'f',[](){ updateSensorDetailMenu(FunctionFavouritePattern); }
  },{
    menuTFunction6, true,'t',[](){ updateSensorDetailMenu(FunctionTriggerSensor); }
  },{
    menuTShowMenu,true,'m',[](){ myMenu.show(); }
  },{
    menuTBackToPrevMenu, true,'.',[](){ toFunctionsMenu(); }
  }
};
constexpr uint8_t functionsDetailMenuSize = GET_MENU_SIZE(functionsDetailMenu);

void toSensorDetailMenu(){
//  setChoosenFunctionTexts();
  myMenu.load(functionsDetailMenu,functionsDetailMenuSize);
  myMenu.show(getFunctionForSensor(currentSensorNo)+1);  
}

// Functions Menu
const char menuTSensor1[] PROGMEM = "Sensor[1]";
const char menuTSensor2[] PROGMEM = "Sensor[2]";
const char menuTSensor3[] PROGMEM = "Sensor[3]";
const char menuTSensor4[] PROGMEM = "Sensor[4]";
#if MAX_SENSORS > 4
const char menuTSensor5[] PROGMEM = "Sensor[5]";
const char menuTSensor6[] PROGMEM = "Sensor[6]";
#endif
const SerialMenuEntry functionsMenu[] = {
  {
    menuTShowMenu,true,'m',[](){ myMenu.show(); }
  },{
    menuTBackToPrevMenu, true,'.',[](){ myMenu.load(sensorMenu, sensorMenuSize); myMenu.show(); }
  },{
    menuTSensor1,   true,'1',[](){ setSensorNo(0); toSensorDetailMenu(); }
  },{
    menuTSensor2,   true,'2',[](){ setSensorNo(1); toSensorDetailMenu(); }
  },{
    menuTSensor3,   true,'3',[](){ setSensorNo(2); toSensorDetailMenu(); }
  },{
    menuTSensor4,   true,'4',[](){ setSensorNo(3); toSensorDetailMenu(); }
  }
  #if MAX_SENSORS > 4
  ,{
    menuTSensor5,   true,'5',[](){ setSensorNo(4); toSensorDetailMenu(); }
  },{
    menuTSensor6,   true,'6',[](){ setSensorNo(5); toSensorDetailMenu(); }
  }
  #endif  
};
//constexpr uint8_t functionsMenuSize = GET_MENU_SIZE(functionsMenu);
#endif

void toFunctionsMenu(){
  myMenu.load(functionsMenu, getNumberOfSensors()+2);
  myMenu.show();
}
const char menuTCalibrate[] PROGMEM = "[C]alibrate";
const char menuTAssignFunctions[] PROGMEM = "Assign [F]unc";
// Sensor Menu
const SerialMenuEntry sensorMenu[] = {
  {
    menuTAmount, true,'a',[](){ amountOfSensors(); }
  },{
    menuTCalibrate, true,'c',[](){ calibrateSensors(); myMenu.show(); }
  },{
    #ifdef MENU_FUNCTIONS
    menuTAssignFunctions, true,'f',[](){ toFunctionsMenu(); }
  },{
    #endif
    menuTShowMenu,true,'m',[](){ myMenu.show(); }
  },{
    menuTBackToPrevMenu, true,'.',[](){ showMain(); }
  }
};
constexpr uint8_t sensorMenuSize = GET_MENU_SIZE(sensorMenu);

void setupMenu(){

  // Set the main menu as the current active menu
  myMenu.load(mainMenu, mainMenuSize);

  // Display the current menu
  myMenu.show();    
}

void updateMenu(){
  if(isMenuActive()){
    myMenu.run();
  }
}
#endif