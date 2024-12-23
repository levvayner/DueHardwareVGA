#include <Print.h>
#ifndef _COMMAND_H
#define _COMMAND_H
#include "commandRequest.h"
class command{
    public:
    char flags[12];
    char name[16];
    char context[16];
    void(*onExecute)(commandRequest request);

    inline void clear(){
        memset(context, 0, sizeof(context));
        memset(name, 0, sizeof(name));
        memset(flags, 0, sizeof(flags));
    }
    template <typename TPort>
    inline void Tostring(TPort port){
        char buf[256];
        sprintf(buf,"Command: %s\nFlags: %s");
        port.println(buf);
    }
};

//class command
#endif