#ifndef _COMMAND_REQUEST_H
#define _COMMAND_REQUEST_H
#include "command.h"
struct commandRequest{
    char *name;
    char flags[12] = {0};
    char args[64] = {0};
    //void(*onExecute)(commandRequest request);
};
#endif