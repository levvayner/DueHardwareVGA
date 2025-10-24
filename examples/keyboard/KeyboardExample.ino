#include "Arduino.h"
#include "DueHardwareVGA.h"

char cmdBuf[256];
uint16_t cmdBufIdx = 0;
unsigned long lastUpdated = 0;
unsigned long checkingTime = 0;

void consoleProcessKey(uint8_t data){
    Serial.print("Received key 0x"); Serial.println(data, HEX);
    cmdBuf[cmdBufIdx++] = data;
    if(data == 10){
        Serial.print("Receieved command: "); Serial.println(cmdBuf);
        memset(cmdBuf,0,sizeof(cmdBuf));
        cmdBufIdx = 0;
    }
}

void setup(){
    Serial.begin(115200);
    Serial.println("Started Due Hardware VGA - Keyboard Demo");
    keyboard.begin(Serial,50);        
    keyboard.onKeyDown = consoleProcessKey;
    
    Serial.println("Initialized");
}


void loop(){
   
}

