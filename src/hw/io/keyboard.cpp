#include "keyboard.h"
VGAKeyboard keyboard;
void vgaKeyboardGetInput(){
    keyboard.getKey();
}

void VGAKeyboard::begin(UARTClass serial,long waitForKeyTimeout, bool waitForNewLine)
{
    _serialInput = serial;
    _ps2Input.begin();
    _waitForKeyTimeout = waitForKeyTimeout;
    _keyPingTimer = new DueTimer(Timer.getAvailable());
    _keyPingTimer->attachInterrupt(vgaKeyboardGetInput);
    _keyPingTimer->start(_waitForKeyTimeout * 1000);
}

char VGAKeyboard::getKey()
{ 
    if(_serialInput.available()){
        char c = _serialInput.read();
        //Serial.println(c, HEX);
        if(c > 0 && onKeyDown != nullptr) onKeyDown(c);
        return c;
                
    }    

    if(_ps2Input.available()){ 
        char c = _ps2Input.read();
        if(c > 0 && onKeyDown != nullptr) onKeyDown(c);
        return c;
    }
    return 0;
}