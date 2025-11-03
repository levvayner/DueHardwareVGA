#include "commandManager.h"

// uint8_t commandManager::registerCommand(const char *name, const char *flags, void (*callback)())
// {
//     return 0;
// }

/// @brief Registers command. Will be called when input matches command
/// @param context Context command is assocaited with. e.g. Console, Editor.
/// @param name Command name/text
/// @param flags Valid flags that can be accepted. e.g. lL to accept -l and -L
/// @param callback 
/// @return 0 if sucessfull. 
byte commandManager::registerCommand(const char * context, const char *name,  const char *flags, void (*callback)(commandRequest request), const char * description)
{
    Serial.print("Registering command: "); Serial.print(name);
    if(_idx >= sizeof(_commands))
        return -1;

    //see if command exists
    for(int idx = 0; idx < _idx; idx++){
        if(strcmp(_commands[idx].context, context) == 0 && strcmp(_commands[idx].name, name) == 0 ){
            //update command with handler, and return
            _commands[idx].onExecute = callback;
            Serial.println(" .. updated");   
            return 0;
        }
    }

    command c;
    memset(c.context,0,sizeof(c.context));
    memcpy(c.context,context,min(strlen(context),sizeof(c.context)));

    memset(c.name,0,sizeof(c.name));
    memcpy(c.name,name,min(strlen(name),sizeof(c.name)));
    
    memset(c.flags,0,sizeof(c.flags));
    memcpy(c.flags,flags,min(strlen(flags),sizeof(c.flags)));

    memset(c.desc,0, sizeof(c.desc));
    memcpy(c.desc, description, min(strlen(description), sizeof(c.desc)));

    c.onExecute = callback;    
    _commands[_idx++] = c;

    Serial.println(" .. added");   
    return 0;
    
}

command* commandManager::getCommand(const char *name)
{
    //search from back of list to get most recently added    
    for(int idx = _idx - 1; idx >= 0; idx--){
        if(strncasecmp(_commands[idx].name, name,strlen(_commands[idx].name)) == 0)
        {
            if(strlen(_commands[idx].name) == strlen(name)) //name will inlcude a trailing null terminator
                return &_commands[idx];
        }   
    }
    return nullptr;
}

command* commandManager::getCommands()
{
    return _commands;
}

commandRequest commandManager::buildCommand(const char* commandText){

    String cmdText = commandText;
    //remove trailing space
    cmdText.trim();
    
    Serial.print("Building command length "); Serial.println(cmdText.length());
    
    uint8_t cmdLength = cmdText.length();
    commandRequest request;
    if(cmdLength == 0) return request;
    
    uint8_t cmdStartIdx = 0;
    uint8_t cmdEndIdx = cmdLength - 1;
    if(cmdText.indexOf(' ') > 0){
        cmdEndIdx = cmdText.indexOf(' ');
    }

    Serial.print("Got response: "); Serial.println(cmdText.c_str());
    Serial.print("End Idx:"); Serial.print(cmdEndIdx); Serial.print(" Cmd Length: "); Serial.println(cmdLength);
    
    if(cmdEndIdx == cmdLength - 1){
        //cmd without  parameters            
        command *cmd = getCommand(cmdText.c_str());
        
        if(cmd == nullptr){
            Serial.print("Unknown command: "); Serial.println(cmdText.c_str());
            request.name = (char *)cmdText.c_str();
            return request;
        }
        request.name = cmd->name;
        request.onExecute = cmd->onExecute;
        request.valid = true;

        return request;
    } else{
        //cmd with parameters
        String cmdTextString = cmdText.substring(cmdStartIdx,cmdEndIdx);
        
        command *cmd = getCommand(cmdTextString.c_str());
        
        if(cmd == nullptr){
            Serial.print("Unknown parametered command: "); Serial.println(cmdTextString.c_str());
            request.name = new char[cmdTextString.length()];
            memcpy(request.name, cmdTextString.c_str(),cmdTextString.length());
            
            return request;
        }
        cmdEndIdx++; // skip space
        
        request.name = cmd->name;
        request.onExecute = cmd->onExecute;
        request.valid = true;
        //bool hasFlags = false;
        for(int flagIdx = cmdEndIdx; flagIdx < cmdLength; flagIdx++){
            if(cmdText.c_str()[flagIdx] == '-' && cmdText.c_str()[flagIdx - 1] == ' '){
                //has flags
                int flagStartIdx = flagIdx;
                int endOfFlagIdx = cmdLength;
                for(int endFlagIdx = flagStartIdx; endFlagIdx < endOfFlagIdx; endFlagIdx++){
                    if(cmdText.c_str()[endFlagIdx] == ' '){
                        endOfFlagIdx = endFlagIdx;
                        break;
                    }
                }
                
                for(int flagIdx = flagStartIdx ;flagIdx < endOfFlagIdx; flagIdx++){
                    if(strchr(cmd->flags, cmdText.c_str()[flagIdx]) != nullptr){
                        request.flags[strlen(request.flags)] = cmdText.c_str()[flagIdx];
                        //snprintf(request.flags,min(sizeof(request.flags), (uint32_t)(endOfFlagIdx - flagStartIdx)),"%s%c", request.flags, resp[flagIdx]);
                    }
                }
                cmdEndIdx = endOfFlagIdx + 1;
                break;
            }         
        }
        
        if(cmdEndIdx > 0 && cmdEndIdx < cmdLength ){
            sprintf(request.args,"%s",cmdText.substring(cmdEndIdx).c_str());
        }
        //cmd->onExecute(request);
        return request;
    }      
}

void commandManager::clearCommands(const char *context)
{
    // _idx is the next available slot
    //memset(_commands,0, sizeof(commandManager)); _idx = 0;
    //first roll through each command from end to start.
    //if record has matching context
    //   if another record is available after it (with different context), starting from end of list, put its data into this record
    //   otherwise clear data
    //   decrement idx 
    for(int idx = _idx - 1; idx >= 0; idx--){
        if(strcmp(_commands[idx].context, context) != 0)
            continue; //not matching context
        
        if(idx < _idx - 1){
            //not last element, swap
            int lastValidOther = _idx - 1;
            for(;lastValidOther >= idx;lastValidOther--){
                if(idx == lastValidOther) break; //did not find it
                if(strcmp(_commands[lastValidOther].context, context) != 0) //found it
                    break;
            }
            if(lastValidOther > idx){
                _commands[idx] = _commands[lastValidOther];
                _commands[lastValidOther].clear();                
            } else{
                _commands[idx].clear(); 
            }
        }else{ //last element
             _commands[idx].clear(); 
        }
        _idx--;
    }
}
commandManager commands;