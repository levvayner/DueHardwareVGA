
#ifndef _VGA_MOUSE_H_
#define _VGA_MOUSE_H_
#include "Arduino.h"
#include "../video/VRAM.h"
#include "DueTimer.h"
#include "../Pins.h"

#include "PS2Mouse.h"
#include "usb.h"
#define DPI 4
enum MousePointer{
    pointerFat = 0,
    pointerSkinny = 1,
    pointerSquare = 2,
    pointerSmall = 3
};
struct MouseClickArgs{
    Point location;
    uint8_t button;
    MouseClickArgs(Point location, uint8_t button){
        this->location = location;
        this->button = button;
    }

    MouseClickArgs(uint16_t x, uint16_t y, uint8_t button){
        location.x = (int)x;
        location.y = (int)y;
        this->button = button;
    }
};
class VGAMouse{
    public:

    inline ~VGAMouse() { 
        if(_initializedPs2 || _initializedUsb){
            _mouseReadTimer->detachInterrupt(); 
        }
        _mouseReadTimer = nullptr;
        _mouse = nullptr;
        _mouseUsb = nullptr;
    }
    //public lifecycle events

    /// @brief Start mouse operations
    /// @param intervalMs how often to query mouse
    void begin(uint16_t intervalMs = 50);
    void end();
    void update();


    void onTick();
    void(*onMouseMove)(int16_t x, int16_t y);
    void(*onMouseDrag)(int16_t x, int16_t y);    
    void(*onClick)(MouseClickArgs args);
    
    
    inline Point location(){ return _mouseLocation;}
    inline void setLocation(uint16_t x, uint16_t y){ 
        _mouseLocation.x = (int)x; 
        _mouseLocation.y = (int)y;
        _pendingMove = true;
    }
    virtual inline MousePointer getPointer(){ return _pointer;}
    virtual inline void setPointer(MousePointer pointer){ _pointer = pointer;}

    virtual inline void RequestRedraw(){
        _pendingRequestRedraw = true;
    }

    MouseController * mouseUsb(){
        return _mouseUsb;
    }
    
    

    protected:

    void drawCursor(int x, int y, int width, int height);
    
    private:
    bool _initializedPs2 = false, _initializedUsb = false;

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
    MouseController * _mouseUsb = nullptr;
    MouseData _lastData;
    bool _pendingEvent = false;
    bool _pendingMove = false;
    bool _pendingRequestRedraw = false;

};
extern VGAMouse mouse;
extern USBHost usb;
#endif
