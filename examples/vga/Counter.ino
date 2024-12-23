#include "Arduino.h"
#include "hw/video/VRAM.h"

VRAM graphics;

unsigned long updateFrequency = 1000;
unsigned long lastUpdated = 0;
unsigned long checkingTime = 0;

void setup(){
    Serial.begin(115200);
    Serial.println("Started Due Hardware VGA");
    graphics.begin();
    graphics.clear();
}


void loop(){
    checkingTime = millis();
    if(checkingTime  - lastUpdated >= updateFrequency){
        
        lastUpdated = checkingTime;
        graphics.clear(0,0,200,10);
        graphics.drawText(1,1, lastUpdated);
    }
}
