#ifndef _PS2_KEYBOARD_H_
#define _PS2_KEYBOARD_H_

#include "Arduino.h"
#include "hw/Pins.h"
//#include "../lib/PS2KeyAdvanced/src/PS2KeyAdvanced.h"
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

// #ifdef PS2_KEY_SEMI
// #undef PS2_KEY_SEMI
// #endif
// #define PS2_KEY_SEMI        0X3B //0x5B

// #ifdef PS2_KEY_COMMA
// #undef PS2_KEY_COMMA
// #endif
// #define PS2_KEY_COMMA       0x2C //0X3B

// #ifdef PS2_KEY_KP_DOT
// #undef PS2_KEY_KP_DOT
// #endif
// #define PS2_KEY_KP_DOT      0x2E // 0x2A

// #ifdef PS2_KEY_KP_ENTER
// #undef PS2_KEY_KP_ENTER
// #endif
// #define PS2_KEY_KP_ENTER    0x10 // 0x2B

// #ifdef PS2_KEY_KP_PLUS
// #undef PS2_KEY_KP_PLUS
// #endif
// #define PS2_KEY_KP_PLUS     0x2B // 0x2C

// #ifdef PS2_KEY_KP_TIMES
// #undef PS2_KEY_KP_TIMES
// #endif
// #define PS2_KEY_KP_TIMES    0x2A // 0x2E


// #ifdef PS2_KEY_SINGLE
// #undef PS2_KEY_SINGLE
// #endif
// #define PS2_KEY_SINGLE      0x60 // 0X40

// #ifdef PS2_KEY_KP_EQUAL
// #undef PS2_KEY_KP_EQUAL
// #endif
// #define PS2_KEY_KP_EQUAL    PS2_KEY_EQUAL //0x3F

// #ifdef PS2_KEY_OPEN_SQ
// #undef PS2_KEY_OPEN_SQ
// #endif
// #define PS2_KEY_OPEN_SQ     0X5B

// #ifdef PS2_KEY_CLOSE_SQ
// #undef PS2_KEY_CLOSE_SQ
// #endif
// #define PS2_KEY_CLOSE_SQ    0X5D
// #define PS2_KEY_QUESTION    0x3F

class ps2KeyboardController{
public:
    void begin();
    virtual inline uint8_t available() { return keyboardPs2.available();}
    virtual inline uint8_t read(){ 
        auto key = getKey();
        if(key.type == None) return 0;
        if(key.type == ConsoleKeyType::ASCII && key.action == KeyDown)        
            return key.keyCode;
        return 0;
    }
    virtual inline ConsoleKeyPress* LastKey(){ return _lastKey;}
    virtual inline void ClearLastKey(){ _lastKey = nullptr;}
    ConsoleKeyPress getKey();

private:

/* mode = 0 echo character
   mode = 1 print string
   mode = 2 cursor movement NO other echo
   mode = 4 ignore key no echo */
    byte _mode = 0;
    byte _idx = 0;
    uint16_t _c;
    ConsoleKeyPress *_lastKey;


};

#endif