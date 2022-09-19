#pragma once
#include <Arduino.h>

boolean trigger(bool &old);

void setupTouch();
void readTouch();
int getTouchRaw(uint8_t);
//int getTouchHold();
//int getTestmodeTriggered();
//boolean isTouchOn();
//void setPatternOn();
bool isTriggered(uint8_t sensorNo);