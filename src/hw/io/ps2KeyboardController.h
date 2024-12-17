#ifndef _PS2_KEYBOARD_H_
#define _PS2_KEYBOARD_H_

#include "Arduino.h"
#include "hw/Pins.h"
#include "PS2KeyAdvanced.h"
#include "sw/Keys.h"


extern PS2KeyAdvanced keyboardPs2;
/* current cursor position */

const uint8_t codes[] = { PS2_KEY_SPACE, PS2_KEY_TAB, PS2_KEY_ESC,
                          PS2_KEY_DELETE, PS2_KEY_F1, PS2_KEY_F2, PS2_KEY_F3,
                          PS2_KEY_F4, PS2_KEY_F5, PS2_KEY_F6, PS2_KEY_F7,
                          PS2_KEY_F8, PS2_KEY_F9, PS2_KEY_F10, PS2_KEY_F11,
                          PS2_KEY_F12 };
const char *const keys[]  =  { " ", "    ", "[ESC]", "[Del]", "[F1]", "[F2]", "[F3]",
                               "[F4]", "[F5]", "[F6]", "[F7]", "[F8]",
                               "[F9]", "[F10]", "[F11]", "[F12]" };
const int8_t sizes[]  = { 1, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5 };



class ps2KeyboardController{
public:
    void begin();
    virtual inline uint8_t available() { return keyboardPs2.available();}
    virtual inline uint8_t read(){ return getKey().keyCode;}
    ConsoleKeyPress getKey();

private:

/* mode = 0 echo character
   mode = 1 print string
   mode = 2 cursor movement NO other echo
   mode = 4 ignore key no echo */
    byte _mode = 0;
    byte _idx = 0;
    uint16_t _c;


};

#endif