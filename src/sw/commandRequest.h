#ifndef _COMMAND_REQUEST_H
#define _COMMAND_REQUEST_H
#include "command.h"
// enum commandSource{
//     sourceSerial = 0,
//     sourcePS2 = 1
// };
struct commandRequest{
    char *name;  
    void(*onExecute)(commandRequest request);
    char flags[12] = {0};
    char args[64] = {0};
    bool valid = false;
    char desc[64] = {0};
    
};
#endif