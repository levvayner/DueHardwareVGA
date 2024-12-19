#ifndef _VGA_KEYBOARD_H
#define _VGA_KEYBOARD_H
#include "Arduino.h"
#include "HardwareSerial.h"
#include "ps2KeyboardController.h"
#include "sw/command.h"
#include "sw/commandManager.h"
#include <typeinfo>

class VGAKeyboard{
    public:    
    inline VGAKeyboard(): _serialInput(Serial), _ps2Input() {};
    void begin(long waitForKeyTimeout = 2000, bool waitForNewLine = false);
    void onTick();

    inline void SetMode(bool processOnEnter){ _waitUntilNewLine = processOnEnter;}


    private:
    template <typename TPort>
    String _getResponse(TPort port){
        String resp;
        unsigned long lastCharTime = millis();
      
        while((millis() - lastCharTime < 2000 && !_waitUntilNewLine )|| true)
        {
            if(port.available()){
                char c = port.read();
                
                if(c == 10 || c == 13) break; //PS2 keyboard uses record seperator

                
                lastCharTime = millis();
                if(c == 0) continue;
                Serial.print("Adding char "); Serial.println(c);
                resp += c;
                
            }
        }
        Serial.print("Got key input: "); Serial.println(resp.c_str());
        resp.trim();
        return resp;
    } 

    template <typename TPort>
    inline void _processInput(TPort port)
    {
        unsigned long startTime = millis();
        while(millis() - startTime < 200 && !port.available());
        //if no data, continue
        if(!port.available()) return;        
        String resp = _getResponse(port);
        if(resp.length() == 0) {
            Serial.println("No response ");
            return;
        }

        commandRequest request;
        //memset(request.flags,0,sizeof(request.flags));
        uint8_t cmdStartIdx = 0;
        uint8_t cmdEndIdx = resp.indexOf(' ');
        
        
        
        Serial.print("Got response: "); Serial.println(resp.c_str());
        Serial.print("End Idx:"); Serial.print(cmdEndIdx); Serial.print(" Cmd Length: "); Serial.println(resp.length());
        if(cmdEndIdx == resp.length() - 1){
            //cmd without  parameters
            command *cmd = commands.getCommand(resp.c_str());
            if(cmd == nullptr){
                Serial.print("Unknown command: "); Serial.println(resp.c_str());
                return;
            }
            request.name = cmd->name;
            cmd->onExecute(request);
            return;
        }else{
            //cmd with parameters
            command *cmd = commands.getCommand(resp.substring(cmdStartIdx,cmdEndIdx).c_str());
            if(cmd == nullptr){
                Serial.print("Unknown command: "); Serial.println(resp.substring(cmdStartIdx,cmdEndIdx).c_str());
                return;
            }
           
            request.name = cmd->name;
            if(resp.substring(cmdEndIdx).indexOf(" -") >= 0){
                Serial.println("Has flag!");
                //has flags
               
                int flagStartIdx = resp.indexOf(" -") + 1;
                int endOfFlagIdx = resp.lastIndexOf(' ') > flagStartIdx ? resp.lastIndexOf(' ') : resp.length();
                for(int flagIdx = flagStartIdx ;flagIdx < endOfFlagIdx; flagIdx++){
                    if(strchr(cmd->flags, resp[flagIdx]) != nullptr){
                        sprintf(request.flags,"%s%c", request.flags, resp[flagIdx]);
                    }
                }
                cmdEndIdx = endOfFlagIdx + 1;
            }
            if(cmdEndIdx > 0 ){
                Serial.println("Has args!");
                sprintf(request.args,"%s",resp.substring(cmdEndIdx).c_str());
            }
            cmd->onExecute(request);
        }
    }

    uint16_t attachDueInterrupt(double microseconds,  void (* callback)(), const char* TimerName);
    private:
    UARTClass _serialInput;
    ps2KeyboardController _ps2Input;  
    bool _waitUntilNewLine = false; 
    //bool _processing = false;


    
    
};
extern VGAKeyboard keyboard;

#endif