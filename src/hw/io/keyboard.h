#ifndef _VGA_KEYBOARD_H
#define _VGA_KEYBOARD_H
#include "Arduino.h"
#include "HardwareSerial.h"
#include "ps2KeyboardController.h"
#include "sw/command.h"
#include "sw/commandRequest.h"
#include "sw/commandManager.h"
#include <typeinfo>
#include "DueTimer.h"
class VGAKeyboard{
    public:    
    inline ~VGAKeyboard() { _keyPingTimer->detachInterrupt(); _keyPingTimer = nullptr;}
    inline VGAKeyboard(): _serialInput(Serial), _ps2Input() {};
    void begin(UARTClass serial, long waitForKeyTimeout = 200, bool waitForNewLine = false); 
    void(*onKeyDown)(uint8_t keyCode);
    char getKey();


    private:
    UARTClass _serialInput;
    ps2KeyboardController _ps2Input; 
    unsigned long  _waitForKeyTimeout = 200;
    DueTimer * _keyPingTimer;
};
extern VGAKeyboard keyboard;

#endif