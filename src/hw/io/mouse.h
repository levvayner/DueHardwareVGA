
#ifndef _VGA_MOUSE_H_
#define _VGA_MOUSE_H_
#include "Arduino.h"
#include "../video/VRAM.h"
#include "DueTimer.h"
#include "../Pins.h"

#include "PS2Mouse.h"
#define USE_USB_MOUSE 0

#include "usb.h"
#define DPI 4
#define CURSOR_SIZE 8
enum MousePointer{
    pointerFat = 0,
    pointerSkinny = 1,
    pointerSquare = 2,
    pointerSmall = 3,
    pointerNone = 4
};
// #if not defined(MouseButton)
// enum MouseButton {
	// LEFT_BUTTON   = 0x01,
	// MIDDLE_BUTTON = 0x02,
	// RIGHT_BUTTON  = 0x04
// };
// #endif

struct MouseClickArgs{
    Point2D location;
    uint8_t button;
    MouseClickArgs(Point2D location, uint8_t button){
        this->location = location;
        this->button = button;
    }

    MouseClickArgs(uint16_t x, uint16_t y, uint8_t button){
        location.x = (int)x;
        location.y = (int)y;
        this->button = button;
    }
};
struct MouseWheelArgs{
    Point2D location;
    int16_t wheelScroll;
    MouseWheelArgs(Point2D location, int16_t wheelScroll){
        this->location = location;
        this->wheelScroll = wheelScroll;
    }
};

class VGAMouse{
    public:
    VGAMouse()
        :   _mouseAreaObject(new GraphicsObject2D( new Rectangle2D(_mouseLocation.x, _mouseLocation.y, CURSOR_SIZE,CURSOR_SIZE),new Texture2D(CURSOR_SIZE,CURSOR_SIZE*4,_mouseCursorBuffer)))
    {
        _initializedPs2 = false;
        _initializedUsb = false;
    }

    inline ~VGAMouse() { 
        if(_initializedPs2 || _initializedUsb){
            _mouseReadTimer->detachInterrupt(); 
        }
        _mouseReadTimer = nullptr;
        _mouse = nullptr;
        #if defined(USE_USB_MOUSE) && USE_USB_MOUSE > 0
        _mouseUsb = nullptr;
        #endif
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
    void(*onWheel)(MouseWheelArgs args);
    
    
    inline Point2D location(){ return _mouseLocation;}
    inline void setPosition(uint16_t x, uint16_t y){ 
        _mouseLocation.x = (int)x; 
        _mouseLocation.y = (int)y;
        _pendingMoveBank1 = true;
        _pendingMoveBank2 = true;
    }
    inline void setPosition(Point2D pos){
        setPosition(pos.x,pos.y);
    } 
    virtual inline MousePointer getPointer(){ return _pointer;}
    virtual inline void setPointer(MousePointer pointer){ _pointer = pointer; _pendingRequestRedraw = true;}

    virtual inline void RequestRedraw(){
        _pendingRequestRedraw = true;
    }
    #if defined(USE_USB_MOUSE) && USE_USB_MOUSE > 0
    MouseController * mouseUsb(){
        return _mouseUsb;
    }
    #endif
    

    protected:

    void drawCursor(int x, int y, int width, int height);
    void clearCursor();
    
    private:
    bool _initializedPs2 = false, _initializedUsb = false;

    private:
    
    Point2D _mouseLocation, _previousLocationBank1, _previousLocationBank2;
    
    GraphicsObject2D * _mouseAreaObject;
    // GraphicsObject2D * _mouseAreaObjectBank1;
    // GraphicsObject2D * _mouseAreaObjectBank2;
    ShapeList<GraphicsObject2D> * _mouseShapes;
    int _zoom = 8;
    char key = 0;
    

    //uint8_t *mouseBuffer = nullptr;
    uint8_t _mouseCursorBuffer[64];
    uint8_t _cursorColor = 0xFF;
    MousePointer _pointer = pointerFat;
    uint8_t _pointers[4][8] = {
        {0x80, 0xE0, 0xF8, 0xFE, 0xF8, 0x1C, 0x0E, 0x06}, //fat
        {0xC0, 0xF0, 0xFC, 0xB0, 0x98, 0x0C, 0x06, 0X03}, //skinny
        {0xF8, 0xE0, 0xF0, 0xB0, 0x98, 0x0C, 0x06, 0X03}, //square
        {0xF0, 0xC0, 0xA0, 0x90, 0x08, 0x04, 0x00, 0x00} //small
    };

    DueTimer * _mouseReadTimer;

    PS2Mouse *_mouse = nullptr;
    #if defined(USE_USB_MOUSE) && USE_USB_MOUSE > 0
    MouseController * _mouseUsb = nullptr;
    #endif
    MouseData _lastData;
    bool _pendingEvent = false;
    bool _pendingMoveBank1 = true;
    bool _pendingMoveBank2 = true;
    bool _pendingRequestRedraw = false;

    bool _pendingRestoreBank1 = false;
    bool _pendingRestoreBank2 = false;

    uint16_t _readInterval = 100;
    private:
    bool tryInitializeMouse(uint8_t clk, uint8_t data);
};
extern VGAMouse mouse;
#if defined(USE_USB_MOUSE) && USE_USB_MOUSE > 0
extern USBHost usb;
#endif
#endif
