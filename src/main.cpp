#include "Arduino.h"
#include "code/execution.h"
#include "hw/video/VRAM.h"
#include "hw/io/ps2KeyboardController.h"
#include "hw/io/keyboard.h"
#include "sw/commandManager.h"
VRAM graphics;

unsigned long updateFrequency = 1000;
unsigned long lastUpdated = 0;
unsigned long checkingTime = 0;
void processLS(commandRequest ls);
void processExit(commandRequest exit);

void setup(){
    Serial.begin(115200);
    Serial.println("Started Due Hardware VGA");
    graphics.begin();
    graphics.clear();

    commands.registerCommand("ls","ldDsS",processLS);
    commands.registerCommand("exit","",processExit);
    keyboard.begin();        
    Serial.println("Initialized");
}


void loop(){
    checkingTime = millis();
    if(checkingTime  - lastUpdated >= updateFrequency){
        
        keyboard.onTick();
        lastUpdated = checkingTime;
        graphics.clear(0,0,200,10);
        graphics.drawText(1,1, lastUpdated);
    }
}

void processLS(commandRequest ls)
{
    Serial.println("Processing ls command");
    Serial.print("with args:  "); Serial.println(ls.args);
    Serial.print("with flags: "); Serial.println(ls.flags);
}

void processExit(commandRequest exit)
{
    Serial.println("Process exit command");
    startApp(startAddress::FLASH0);
}
