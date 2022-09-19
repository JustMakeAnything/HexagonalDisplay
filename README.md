# HexagonalDisplay

A LED display which uses hexagonal instead of square pixels

Arduino Software to control LED Strips aka Neopixels on a 2D hexagonal display


## Configuration

The software has a constants.h file where some configurations can be done. Usually the ones which affect
static memory consumption. Some changes have to be done in the coding.

All other configurations can be done menu driven via the serial interface. Open the serial console at 9600 bps.
It includes
* Amount of LEDs 
* Amount of sensors
* Sensor calibration
* Assignment of functions to sensors
* Pattern selection
