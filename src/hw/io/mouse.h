
#ifndef _VGA_MOUSE_H_
#define _VGA_MOUSE_H_
#include "Arduino.h"
#include "../video/VRAM.h"
#include "DueTimer.h"
#include "../Pins.h"

#include "PS2Mouse.h"
enum MousePointer{
    pointerFat = 0,
    pointerSkinny = 1,
    pointerSquare = 2,
    pointerSmall = 3
};
class VGAMouse{
    public:

    inline ~VGAMouse() { 
        if(_initialized){
            _mouseReadTimer->detachInterrupt(); 
        }
        _mouseReadTimer = nullptr;
        _mouse = nullptr;
    }
    //public lifecycle events

    /// @brief Start mouse operations
    /// @param intervalMs how often to query mouse
    void begin(uint16_t intervalMs = 50);
    void end();
    void update();


    void onTick();
    void(*onMouseMove)();
    void(*onClick)(uint8_t button);
    
    
    inline Point location(){ return _mouseLocation;}
    virtual inline MousePointer getPointer(){ return _pointer;}
    virtual inline void setPointer(MousePointer pointer){ _pointer = pointer;}
    
    

    protected:

    void drawCursor(int x, int y, int width, int height);
    
    private:
    bool _initialized = false;

    private:
    
    Point _mouseLocation, _previousLocation;
    int _zoom = 8;
    char key = 0;
    

    uint8_t *mouseBuffer = nullptr;
    uint8_t _mouseCursorBuffer[64];
    MousePointer _pointer = pointerFat;
    uint8_t _pointers[4][8] = {
        {0x80, 0xE0, 0xF8, 0xFE, 0xF8, 0x1C, 0x0E, 0x06}, //fat
        {0xC0, 0xF0, 0xFC, 0xB0, 0x98, 0x0C, 0x06, 0X03}, //skinny
        {0xF8, 0xE0, 0xF0, 0xB0, 0x98, 0x0C, 0x06, 0X03}, //square
        {0xF0, 0xC0, 0xA0, 0x90, 0x08, 0x04, 0x00, 0x00} //small
    };

    DueTimer * _mouseReadTimer;

    PS2Mouse *_mouse = nullptr;
    MouseData _lastData;
    bool _pendingEvent = false;
    bool _pendingMove = false;

};
extern VGAMouse mouse;
#endif
