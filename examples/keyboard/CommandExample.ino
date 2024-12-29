#include "Arduino.h"
#include "DueHardwareVGA.h"

VRAM graphics;
char cmdBuf[256];
uint16_t cmdBufIdx = 0;
unsigned long updateFrequency = 200;
unsigned long lastUpdated = 0;
unsigned long checkingTime = 0;
bool commandReady = false;
void consoleProcessKey(uint8_t data){
    Serial.print("Received key 0x"); Serial.println(data, HEX);
    if(data == 13) return; //ignore carriage return, new line advances to beginning of line
    if(data == 10){
        commandReady = true;
    }
    
    cmdBuf[cmdBufIdx++] = data;
    
}
void processLS(commandRequest ls);
void processExit(commandRequest exit);

void setup(){
    Serial.begin(115200);
    Serial.println("Started Due Hardware VGA");
    graphics.begin();
    graphics.clear();

    commands.registerCommand("COMMAND_TEST", "ls","ldDsS",processLS);
    commands.registerCommand("COMMAND_TEST", "exit","",processExit);
    keyboard.begin(Serial,50);        
    keyboard.onKeyDown = consoleProcessKey;
    
    Serial.println("Initialized");
}


void loop(){
    if(commandReady){
        //Serial.print("Receieved command: "); Serial.println(cmdBuf);
        //check if registered command
        auto command = commands.buildCommand(cmdBuf);
        if(command.valid){
            command.onExecute(command);
        }

        memset(cmdBuf,0,sizeof(cmdBuf));
        cmdBufIdx = 0;
        commandReady = false;
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
    startApp(startAddress::FLASH0, Serial);
}
