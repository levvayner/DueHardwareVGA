#include "keyboard.h"
//#include "SAMDUETimerInterrupt.h"
VGAKeyboard keyboard;
String serialInputText = "";
String ps2InputText = "";
void vgaKeyboardGetInput(){
    keyboard.onTick();
}


void VGAKeyboard::begin(long waitForKeyTimeout, bool waitForNewLine)
{
    _ps2Input.begin();
    //_serialInput.begin(115220);
    //pmc_enable_periph_clk (TC_INTERFACE_ID + 0*3+0) ;  // clock the TC0 channel 0
    //attachDueInterrupt(20000, vgaKeyboardGetInput, "Keyboard");
}

void VGAKeyboard::onTick()
{
  
        _processInput(Serial);  
        _processInput(_ps2Input);

        if(_ps2Input.LastKey() != nullptr){
            auto key = *_ps2Input.LastKey();
            if(key.isAltPressed){
                switch (key.keyCode)
                {
                case PS2_KEY_D:
                    Serial.println("Execute Alt + D action");
                    break;
                
                default:
                    break;
                }
            }
            if(key.isCtrlPressed){
                switch (key.keyCode)
                {
                case PS2_KEY_D:
                    Serial.println("Execute Ctrl + D action");
                    break;
                
                default:
                    break;
                }
            }
            _ps2Input.ClearLastKey();
        }

}
uint16_t VGAKeyboard::attachDueInterrupt(double microseconds, void (* callback)(), const char *TimerName)
{
    
//   DueTimerInterrupt dueTimerInterrupt = DueTimer.getAvailable();
  
//   dueTimerInterrupt.attachInterruptInterval(microseconds, (void (*)())callback);

//   uint16_t timerNumber = dueTimerInterrupt.getTimerNumber();
  
//   Serial.print(TimerName); Serial.print(F(" attached to Timer(")); Serial.print(timerNumber); Serial.println(F(")"));

//   return timerNumber;

}
