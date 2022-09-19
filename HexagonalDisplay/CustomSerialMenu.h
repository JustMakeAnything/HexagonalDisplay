#include<Arduino.h>
#define SERIALMENU_MINIMAL_FOOTPRINT true

#if SERIALMENU_DISABLE_PROGMEM_SUPPORT != true
constexpr PROGMEM char SERIAL_MENU_COPYRIGHT[] = 
#else
constexpr char SERIAL_MENU_COPYRIGHT[] = 
#endif
///////////////////////////////////////////////////////////////////////////////
// Adaption of the SerialMenu library from Dan Truong 
// licenced by MIT licence
// MIT License

// Copyright (c) 2019 Dan Truong

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// SerialMenu - An Efficient Menu Library for Arduino Serial Console
"";//"SerialMenu - Copyright (c) 2019 Dan Truong";
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Define a menu entry as:
// - a menu message to display
// - a boolean to specify if the message is in SRAM or PROGMEM Flash memory
// - a menu key to select
// - a callback function to perform the menu action
///////////////////////////////////////////////////////////////////////////////
class SerialMenuEntry {

  private:
    // Message to display via getMenu()
    // The pointer can be in SRAM or in FLASH (requires PROGMEM to access)
    const char * message;
    // Keyboard character entry to select this menu entry, overloaded:
    // We set bit 0x20 to 0 for normal message, to 1 for a PROGMEM message
    const char key;
  public:
    void (*actionCallback)();
  public:
    // Constructor used to init the array of menu entries
    SerialMenuEntry(const char * m, bool isprogMem, char k, void (*c)()) :
      message(m),
      #if SERIALMENU_DISABLE_PROGMEM_SUPPORT != true
        key(((isprogMem) ? (k|0x20) : (k&(~0x20)))),
      #else
        key(k),
      #endif
      actionCallback(c)
    {}
  
    // Get the menu message to display
    inline const char * getMenu() const{
      return message;
    }

    inline bool isProgMem() const{
      return key & 0x20;
    }

    // Check if the user input k matches this menu entry
    // Characters are converted to lowecase ASCII.
    // @note this impacts also symbols, not numbers, so test before using those
    inline bool isChosen(const char k) const{
      return (k|0x20) == (key|0x20);
    }
};

///////////////////////////////////////////////////////////////////////////////
// Macro to get the number of menu entries in a menu array.
///////////////////////////////////////////////////////////////////////////////
#define GET_MENU_SIZE(menu) sizeof(menu)/sizeof(SerialMenuEntry)


///////////////////////////////////////////////////////////////////////////////
// The menu is a singleton class in which you load an array of menu entries.
//
// Singleton:
// In other words, you do not instantiate the class. Instead you declare a
// pointer to the class and ask the class to allocate an instance of itself
// once, and return a pointer to the only static instance the program has.
// A call to SerialMenu::get() will always return the same pointer to that
// one single instance.
//
///////////
// Example:
///////////
// loop()
// {
//    SerialMenu& menu = SerialMenu::get();
// }
// @todo Instead of a singleton we could avoid having any instance and
// convert methods to static methods using static data. It saves a pointer...
///////////////////////////////////////////////////////////////////////////////
class SerialMenu{
  private:
    // If PROGMEM is used, copy using this SRAM buffer size.
    static constexpr uint8_t PROGMEM_BUF_SIZE = 8;

    // This class implements a singleton desgin pattern with one static instance
    static SerialMenu * singleton;

    // Points to the array of menu entries for the current menu
    static const SerialMenuEntry * menu;
    // number of entries in the current menu
    static uint8_t size;

    // Private constructor for singleton design.
    // Initializes with an empty menu, prepares serial console and staus LED.
    SerialMenu()
    {
      Serial.begin(9600);
      while (!Serial){};
    }

  public:
    // Get a pointer to the one singleton instance of this class
    static SerialMenu & get()
    {
      if (singleton == nullptr)
      {
        singleton = new SerialMenu;
      }
      return *singleton;
    }

    // Get a pointer to the one singleton instance of this class and point it
    // to the current menu
    static const SerialMenu & get(const SerialMenuEntry* array, uint8_t arraySize)
    {
      (void) SerialMenu::get();
      singleton->load(array, arraySize);
      return *singleton;
    }
    
    // Install the current menu to display
    inline void load(const SerialMenuEntry* array, uint8_t arraySize) const
    {
      menu = array;
      size = arraySize;
    }

    void show() const{
      show(0);
    }
    // void showSensors() const{
    //   char menuTSensor1[] = "Sensor[1]";      
    //   SerialMenuEntry functionsMenu[3] = { { menuTSensor1,false,'1',[](){ callback(); }}};
    //   for (uint8_t i = 0; i < 3; i++){
    //     functionsMenu[i] = { menuTSensor1,false,'1',[](){ NULL };}
    //   }
      

    // }
    // Display the current menu on the Serial console
    void show(uint8_t mark) const{
      #if SERIALMENU_MINIMAL_FOOTPRINT != true
      Serial.println("\nMenu:");
      #else
      Serial.println();
      #endif

      for (uint8_t i = 0; i < size; ++i)
      {
      #if SERIALMENU_DISABLE_PROGMEM_SUPPORT != true
        if (menu[i].isProgMem())
        {
          // String in PROGMEM Flash, move it via a SRAM buffer to print it
          char buffer[PROGMEM_BUF_SIZE];
          const char * progMemPt = menu[i].getMenu();
          uint8_t len = strlcpy_P(buffer, progMemPt, PROGMEM_BUF_SIZE);
          if(mark>0){
            if(mark==i+1){
              Serial.print(">");
            }else{
              Serial.print(" ");
            }
          }
          Serial.print(buffer);
          while (len >= PROGMEM_BUF_SIZE)
          {
            len -= PROGMEM_BUF_SIZE - 1;
            progMemPt += PROGMEM_BUF_SIZE - 1;
            // @todo replace strlcpy_P() and buffer with moving a uint32?
            len = strlcpy_P(buffer, progMemPt, PROGMEM_BUF_SIZE);
            Serial.print(buffer);
          }
          Serial.println("");
        }
        else
      #endif
        {
          // String in data SRAM, print directly
          Serial.println(menu[i].getMenu());
        }
      }
    }

    // return a single ASCII character input read form the serial console.
    // Note: this routine is blocking execution until a number is input
    inline char getChar()
    {
      while (!Serial.available());
      return Serial.read();
    }

    // return a number input read form the serial console.
    // Note: this routine is blocking execution until a number is input
    template <class T>
    inline T getNumber(const char * const message = nullptr)
    {
      T value = 0;
      bool isNegative = false;
      T decimals = 0;
      
      if (message)
      {
        Serial.print(message);
      }
      char c = '0';
      
      // Skip the first invalid carriage return
      while (!Serial.available());
      c = Serial.read();
      if (c == 0x0A)
      {
        while (!Serial.available());
        c = Serial.read();
      }

      if (c == '-')
      {
        isNegative = true;
        while (!Serial.available());
        c = Serial.read();
      }
      
      while ((c >= '0' and c <= '9') || c == '.')
      {
        decimals *= 10;

        if (c >= '0' and c <= '9')
        {
          value = value * 10 + (c - '0');
        }
        else if (decimals == 0 && c == '.')
        {
          decimals = 1;
        }

        while (!Serial.available());
        c = Serial.read();
      }
      
      if (isNegative)
      {
        value = -value;
      }
      
      if (decimals)
      {
        value /= decimals;
      }
      
      if (message)
      {
        Serial.println(value);
      }
      return value;
    }


///////////////////////////////////////////////////////////////////////////////
    // run the menu. If the user presses a key, it will be parsed, and trigger
    // running the matching menu entry callback action. If not print an error.
    // Returns false if there was no menu input, true if there was
    bool run()
    {
      const bool userInputAvailable = Serial.available();
      // Process the input
      if (!userInputAvailable){
        return false;
      }else{
        // Read one character from the Serial console as a menu choice.
        char menuChoice = Serial.read();
        
        // Carriage return is not a menu choice
        if (menuChoice == 0x0A)
        {
          return false;
        }
       
        uint8_t i;
        for (i = 0; i < size; ++i)
        {
          if (menu[i].isChosen(menuChoice))
          {
            menu[i].actionCallback();
            break;
          }
        }
        if (i == size)
        {
          Serial.print(menuChoice);
          Serial.println(": Invalid menu choice.");
        }
        return true;
      }
    }

};