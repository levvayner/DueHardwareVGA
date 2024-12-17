#include "commandManager.h"

// uint8_t commandManager::registerCommand(const char *name, const char *flags, void (*callback)())
// {
//     return 0;
// }

byte commandManager::registerCommand(const char *name, const char *flags, void (*callback)(commandRequest request))
{
    Serial.print("Registering command: "); Serial.print(name);
    if(_idx >= sizeof(_commands))
        return -1;
    
    command c;
    memset(c.name,0,sizeof(c.name));
    memcpy(c.name,name,min(strlen(name),sizeof(c.name)));
    
    memset(c.flags,0,sizeof(c.flags));
    memcpy(c.flags,flags,min(strlen(flags),sizeof(c.flags)));
    c.onExecute = callback;
    _commands[_idx++] = c;

    Serial.println(" .. done");   
    
    
}

command* commandManager::getCommand(const char *name)
{
    Serial.print("Searching for command: "); Serial.println(name);
    for(int idx = 0; idx < _idx; idx++){
        if(strcasecmp(_commands[idx].name, name) == 0 )
            return &_commands[idx];
    }
    return nullptr;
}
commandManager commands;