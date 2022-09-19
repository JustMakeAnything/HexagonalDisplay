#include <Adafruit_NeoPixel.h>

// The type of strip can be defined here (with: NEO+GRBW or without white: NEO+GRB)
// Sequence of colors can be changed: GRB is common, but other like BGR are also possible 
// R=Red G=Green B=Blue
#define STRIP_TYPE NEO_GRB
// These numbers influence the static memory consumption
#define MAX_SENSORS 4  // Up to 6 possible
#define MAXPATTERNS 7


// Pattern max values
#define MAXPOSITIONS 10  // Patterns can use this much elements
#define MAX_NUM_LEDS 120 // only byte values are possible max(255) Memory restriction


// Conditional compiling to save memory
#define MENU
#define MENU_LED_STRIP
#define MENU_PATTERNS
#define MENU_FUNCTIONS
//#define MENU_LED_ORDER   // Work in progress...
// Save memory, but texts may be more cryptic
//#define LOWMEM x

#define DEFAULT_NUM_LEDS 10
// How many leds are supported by configuration
// For Atmega 168 MAX_LED_STRIP_COUNT * MAX_NUM_LEDS cannot exceed 30,
// otherwise the software will crash
#define MAX_LED_STRIP_COUNT 1 // One for 2D only



#define LED2D
// When defined, there is a 2D matrix
// Currently a hegagonal matrix
// The following is the layout:
// Start is bottom left
// Every second line has a LED less
//
// Example for X=5 Y=5:
//
// 4 X X X X X
// 3  X X X X
// 2 X X X X X
// 1  X X X X
// 0 X X X X X
//   0 1 2 3 4
// Led size is the amount of leds in the display
#define LED_SIZE_X 9
#define LED_SIZE_Y 13
//#define FIRSTSHORT 1   // Not implemented yet !!! Set this to 1 when the first row is short
#define INPUTRIGHT     // The input to the panel is on the right side

// Display size is the physical size
// Accuracy is 1/10 mm i.e. 13.33 cm = 1333
// This is more or less the aspect ratio helper
//#define DISPLAY_SIZE_X 1333
//#define DISPLAY_SIZE_Y 1125
// To have more accuracy, the display values are used internally in a higer
// Resolution. Usually we want to have the values in a range of up to 2^14 = 16384
// TODO: Do this automatically?
// TODO: use 10000 instead to make it easier to understand in pattern programming?
// TODO: Think about a timebase (Cycles per second fixed, not dependent on sensors)
//#define DISPLAY_FACTOR 8  //15996 / 13500
// 8 = ( 9331 / 9000 )
#define DISPLAY_SIZE_X 2660   
#define DISPLAY_SIZE_Y 3430
#define DISPLAY_FACTOR 3 
//
// The aspect ratio is determined from the amount of LEDS
// 
// Discs should always be a circle
// Positions are just "walked" trough.
// When the edge is reached, it continues on the
// oposite site. 
// It can be configured if edges should be wrap around or not
//


// Data pins of both strips
#define DATA_PIN1 2
#define DATA_PIN2 3

// Touch reaction time
#define TOUCH_CYCLES 1

// Pin which triggers the mosfet to disable the led strips
// This is needed to save power since each led draws about 1ma when switched off
// Currently not implemented
#define LED_ENABLE 11

// Only for alternative touch library
// Pin where all 2 MOhm resistors are connected
#define TOUCH_COMMON 5

// Uncomment DEBUG to get information on the serial console
// Probably not possible for Atemga 168 when other options are active
// due to memory issues
//#define DEBUG

// Change this value to any other value to reset the configuration
#define MAGICCODE 113