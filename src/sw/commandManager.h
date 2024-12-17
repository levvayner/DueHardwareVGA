#if !defined(_COMMAND_MANAGER_H)
#define _COMMAND_MANAGER_H
#include "command.h"
#include "Arduino.h"

class commandManager{
    public:
    byte registerCommand(const char * name, const char* flags, void (*callback)(commandRequest request) );
    command* getCommand(const char * name);
    
    private:
    command _commands[32] = {};
    uint8_t _idx = 0;
    
};
extern commandManager commands;

#endif // _COMMAND_MANAGER_H
