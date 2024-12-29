#include "mouse.h"
USBHost usb;
VGAMouse mouse;
extern char buf[64];
void vgaMouseInputEventHandler(){
    mouse.onTick();
}

void mouseMoved(){   
    
    auto usbMouse = *mouse.mouseUsb();
    int16_t newX = usbMouse.getXChange() / DPI;
    int16_t newY = usbMouse.getYChange() / DPI;
   
    // sprintf(buf,"moving mouse to (%d,%d)",
    //    newX, newY
    // );
    // Serial.println(buf);

    mouse.setLocation(newX, newY);
    if(mouse.onMouseMove != nullptr)
        mouse.onMouseMove(newX, newY);    
}

void mousePressed(){
    auto usbMouse = *mouse.mouseUsb();
    //Serial.println("Mouse Pressed");
    uint8_t button = 0;
    button  |= usbMouse.getButton(MouseButton::LEFT_BUTTON);
    button  |= (usbMouse.getButton(MouseButton::MIDDLE_BUTTON) << 1);
    button  |= (usbMouse.getButton(MouseButton::RIGHT_BUTTON) << 2);
    if(mouse.onClick != nullptr)
        mouse.onClick(MouseClickArgs(mouse.location(), button));
}

void mouseDragged(){
    auto usbMouse = *mouse.mouseUsb();
    int16_t newX = usbMouse.getXChange() / DPI;
    int16_t newY = usbMouse.getYChange() / DPI;
    // sprintf(buf,"dragging mouse to (%d,%d)",
    //    newX, newY
    // );
    // Serial.println(buf);
    //mouse.setLocation(newX, newY);
    if(mouse.onMouseDrag != nullptr)
        mouse.onMouseDrag(newX, newY); 
    
}

void VGAMouse::begin(uint16_t intervalMs)
{   
    if(!_initializedPs2){
        _mouse = new PS2Mouse(PS2_CLK2, PS2_DATA2);
        if(_mouse->begin()) {
            _mouseReadTimer = new DueTimer(Timer.getAvailable());
            _mouseReadTimer->attachInterrupt(vgaMouseInputEventHandler);
            _mouseReadTimer->start(intervalMs * 1000);
            _initializedPs2 = true;
            Serial.println("Initialized PS2 Mouse");
        }
    }
    #if defined(USE_USB_MOUSE) && USE_USB_MOUSE > 0
    if(!_initializedUsb){
        _mouseUsb = new MouseController(usb);
        if(_mouseUsb != nullptr){
            _initializedUsb = true;
            Serial.println("Initialized USB Mouse");
        }
    }     
    #endif
    
    this->_pendingEvent = true;
}

void VGAMouse::end()
{
    _mouse = nullptr;
    if(_initializedPs2){
        _mouseReadTimer->stop();
        _mouseReadTimer->detachInterrupt();
    }
    _mouseReadTimer = nullptr;
    _initializedPs2 = false;
    _initializedUsb = false;
}

void VGAMouse::drawCursor(int x, int y, int width, int height)
{
    if(!_initializedPs2 && !_initializedUsb) return;
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
    if(!_initializedPs2 && !_initializedUsb) return;
    bool pendingEvent = false;
    bool pendingMove = false;
    uint8_t buttonClicked = 0;

    if(_initializedPs2){
        //ps2
        _lastData =_mouse->readData();
        buttonClicked = _lastData.status & 0x7;
        
        pendingEvent = buttonClicked > 0; //any lower 3 bits set
        pendingMove = _lastData.position.x != 0 || _lastData.position.y != 0;
    }

    //usb 
    if(_initializedUsb){
        pendingEvent = _pendingEvent;
        pendingMove = _pendingMove;
    }   

    if(!pendingEvent && !pendingMove) return;
    
    // char buf[128] = {0};
    // sprintf(buf, "Mouse flags %02X, Move (%d,%d)", _lastData.status, _lastData.position.x, _lastData.position.y);
    // Serial.print(buf);    

    if(pendingEvent){
        //left mouse button  
        if(onClick != nullptr)
            onClick(MouseClickArgs(_mouseLocation, buttonClicked));
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
            onMouseMove(_lastData.position.x, _lastData.position.y);
    }
    _pendingMove = pendingMove;
    _pendingEvent = pendingEvent;
}

void VGAMouse::update()
{
    if(!_initializedPs2 && !_initializedUsb) return;
    if(_pendingEvent || _pendingMove || _pendingRequestRedraw){
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
        _pendingRequestRedraw = false;
    }
    
}
