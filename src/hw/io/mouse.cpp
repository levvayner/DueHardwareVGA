#include "mouse.h"
VGAMouse mouse;
void vgaMouseInputEventHandler(){
    mouse.onTick();
}

void VGAMouse::begin(uint16_t intervalMs)
{   
    if(!_initialized){
        _mouse = new PS2Mouse(PS2_CLK2, PS2_DATA2);
        if(!_mouse->begin()) return;
        _mouseReadTimer = new DueTimer(Timer.getAvailable());
        _mouseReadTimer->attachInterrupt(vgaMouseInputEventHandler);
        _mouseReadTimer->start(intervalMs * 1000);
        _initialized = true;
    }   
    
    update();
}

void VGAMouse::end()
{
    _mouse = nullptr;
    _mouseReadTimer->stop();
    _mouseReadTimer->detachInterrupt();
    _mouseReadTimer = nullptr;
    _initialized = false;
}

void VGAMouse::drawCursor(int x, int y, int width, int height)
{
    memcpy(_mouseCursorBuffer, mouseBuffer, width*height);
        for(int line=0;line<height; line++){
        uint8_t cursorRowPixels = _pointers[(int)_pointer][line];
        for(int row=0;row < width; row++){
            if(cursorRowPixels & (0x1 << (7-row)))
                _mouseCursorBuffer[(line*width) + row] = 255;
        }
    }
    if(_mouseCursorBuffer != nullptr){
            graphics.drawBuffer(_mouseLocation.x, _mouseLocation.y, width,height, _mouseCursorBuffer, btVertical);
        
    }
}

void VGAMouse::onTick()
{
    _lastData =_mouse->readData();
    
    bool pendingEvent = (_lastData.status & 0x7 ) > 0; //any lower 3 bits set
    bool pendingMove = _lastData.position.x != 0 || _lastData.position.y != 0;

    if(!pendingEvent && !pendingMove) return;
    
    // char buf[128] = {0};
    // sprintf(buf, "Mouse flags %02X, Move (%d,%d)", _lastData.status, _lastData.position.x, _lastData.position.y);
    // Serial.print(buf);    

    if(pendingEvent){
        //left mouse button  
        if(onClick != nullptr)
            onClick(0);
    }

    if(pendingMove){
        if(!_pendingMove) // update previous location is not already pending a move
            _previousLocation = Point(_mouseLocation.x, _mouseLocation.y);
        //update position
        if(_lastData.position.x > 127 ){
            _mouseLocation.x -= (256 - _lastData.position.x );
            if(_mouseLocation.x < 0) 
                _mouseLocation.x = 0;
        }else{
            _mouseLocation.x += _lastData.position.x;
            if(_mouseLocation.x > graphics.settings.screenWidth) 
                _mouseLocation.x = graphics.settings.screenWidth - 2;
        }
        if(_lastData.position.y > 127){
            _mouseLocation.y += (256 - _lastData.position.y );
            if(_mouseLocation.y > graphics.settings.screenHeight) 
                _mouseLocation.y = graphics.settings.screenHeight - 2;        
            
        }else{
            _mouseLocation.y -= _lastData.position.y;
            if(_mouseLocation.y < 0) 
                _mouseLocation.y = 0;
        }
        if(onMouseMove != nullptr)
            onMouseMove();
    }
    _pendingMove = pendingMove;
    _pendingEvent = pendingEvent;
}

void VGAMouse::update()
{
    if(_pendingEvent || _pendingMove){
        //write out the old
        if(mouseBuffer != nullptr){
            
            graphics.drawBuffer(_previousLocation.x, _previousLocation.y, _zoom, _zoom, mouseBuffer, btVertical);           
        } else if(mouseBuffer == nullptr){         
            mouseBuffer = new uint8_t[_zoom*_zoom];        
        }

        if(_pendingMove)_previousLocation = _mouseLocation; //if we moved, update location
        //read in buffer
        graphics.readBuffer(_mouseLocation, _zoom, _zoom, mouseBuffer, btVertical);

        //draw cursor
        drawCursor(_mouseLocation.x, _mouseLocation.y, _zoom, _zoom);
        _pendingEvent = false;
        _pendingMove = false;
        
    }
}
