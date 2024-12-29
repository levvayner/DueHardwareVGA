#if !defined(_COMMAND_MANAGER_H)
#define _COMMAND_MANAGER_H
#include "command.h"
#include "Arduino.h"

class commandManager{
    public:
    uint8_t registerCommand(const char* context, const char * name, const char* flags, void (*callback)(commandRequest request) );
    command* getCommand(const char * name);
    inline command* getCommand(uint8_t index){ return &_commands[index];}
    commandRequest buildCommand(const char* cmd);
    inline void clearCommands(){ memset(_commands,0, sizeof(commandManager)); _idx = 0;}
    void clearCommands(const char* context);
    inline uint8_t commandCount(){ return _idx;}
    private:
    command _commands[64] = {};
    uint8_t _idx = 0;
    
};
extern commandManager commands;

#endif // _COMMAND_MANAGER_H
