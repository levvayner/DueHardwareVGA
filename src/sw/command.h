#include <Print.h>
#ifndef _COMMAND_H
#define _COMMAND_H
#include "commandRequest.h"
class command{
    public:
    char flags[12];
    char name[16];

    void(*onExecute)(commandRequest request);

    template <typename TPort>
    inline void Tostring(TPort port){
        char buf[256];
        sprintf(buf,"Command: %s\nFlags: %s");
        port.println(buf);
    }
};

//class command
#endif;