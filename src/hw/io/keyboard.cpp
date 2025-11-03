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
        auto keyPress = _ps2Input.getKey();
        char c = 0;//_ps2Input.read();
        Serial.print("Got PS2 Key: 0x"); Serial.print(c, HEX); Serial.print(" type: "); Serial.println(keyPress.type);
        if(keyPress.type == ASCII || keyPress.type == Cursor)
            c = keyPress.keyCode;

        if(c > 0 && onKeyDown != nullptr) onKeyDown(c);
        return c;
    }
    return 0;
}