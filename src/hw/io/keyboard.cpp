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
    //_serialInput.begin(115220);
    //pmc_enable_periph_clk (TC_INTERFACE_ID + 0*3+0) ;  // clock the TC0 channel 0
    //attachDueInterrupt(20000, vgaKeyboardGetInput, "Keyboard");
}

void VGAKeyboard::onTick()
{
    // if(!_processing){
    //     _processing = true;
        //Serial.println("Processing Serial");
        _processInput(Serial);
        //if(_ps2Input.available())
        Serial.println("Processing PS2");
        _processInput(_ps2Input);
    //     _processing = false;
    // }

}
uint16_t VGAKeyboard::attachDueInterrupt(double microseconds, void (* callback)(), const char *TimerName)
{
    
//   DueTimerInterrupt dueTimerInterrupt = DueTimer.getAvailable();
  
//   dueTimerInterrupt.attachInterruptInterval(microseconds, (void (*)())callback);

//   uint16_t timerNumber = dueTimerInterrupt.getTimerNumber();
  
//   Serial.print(TimerName); Serial.print(F(" attached to Timer(")); Serial.print(timerNumber); Serial.println(F(")"));

//   return timerNumber;

}
