#include "mouse.h"
#include <sw/GPU.h>
#if defined(USE_USB_MOUSE) && USE_USB_MOUSE > 0
USBHost usb;
#endif
VGAMouse mouse;
extern GPU gpu;
extern char buf[64];
void vgaMouseInputEventHandler(){
    mouse.onTick();
}

#if defined(USE_USB_MOUSE) && USE_USB_MOUSE > 0
void mouseMoved(){   
    
    auto usbMouse = *mouse.mouseUsb();
    int16_t newX = usbMouse.getXChange() / DPI;
    int16_t newY = usbMouse.getYChange() / DPI;
   
    sprintf(buf,"moving mouse to (%d,%d)",
       newX, newY
    );
    Serial.println(buf);

    mouse.setPosition(newX, newY);
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
    //mouse.setPosition(newX, newY);
    if(mouse.onMouseDrag != nullptr)
        mouse.onMouseDrag(newX, newY); 
    
}
#endif
void VGAMouse::begin(uint16_t intervalMs)
{   
    _readInterval = intervalMs;
    _mouseShapes = new ShapeList<Graphics2DObject>();
    if(!_initializedPs2){        
        //try usb port
        Serial.print("Initialing PS2 Mouse.. using top USB port..");
        _initializedUsb = tryInitializeMouse(USB_CLK2, USB_DATA2);
        if(!_initializedUsb){
            Serial.print(" failed! Trying PS2 port ...");
            _initializedPs2 = tryInitializeMouse(PS2_CLK,PS2_DATA);
        }
        if(!_initializedPs2 && !_initializedUsb){
            Serial.println("Failed to initialize mouse!");            
        }else
            Serial.println(" Initialized!");
            
        //_mouseArea = new Rectangle2D(_mouseLocation.x, _mouseLocation.y, 8,8);
        //_mouseAreaObject =  Graphics2DObject(new Rectangle2D(_mouseLocation.x, _mouseLocation.y, _mouseLocation.x+ 8, _mouseLocation.x + 8), new Texture2D(8,8,_mouseCursorBuffer));
        _mouseShapes->push_back(*_mouseAreaObject);
        gpu.SetMouseCanvas(_mouseShapes);
    }
    #if defined(USE_USB_MOUSE) && USE_USB_MOUSE > 0
    if(!_initializedUsb){
        Serial.print("Initializing mouse using native USB...");
        _mouseUsb = new MouseController(usb);
        if(_mouseUsb != nullptr){
            _initializedUsb = true;
            Serial.println(" Initialized USB Mouse");            
        } else{
            Serial.println(" Failed!");
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
    //auto _mouseAreaObject = gpu.activeBank() ? _mouseAreaObjectBank2 : _mouseAreaObjectBank1;
    bool activeBank = gpu.activeBank();    
    Rectangle2D objBounds = _mouseAreaObject->getBounds();
    bool pendingMove = activeBank ? _pendingMoveBank2 : _pendingMoveBank1;
    int bufferOldOffset = activeBank * height * width * 2;
    int bufferCursorOffset = (activeBank * height * width * 2) + (height * width);
    
    // Serial.print("Bounds: "); Serial.print(objBounds.x1());Serial.print(", "); Serial.print(objBounds.y1());    
    // Serial.print(" - W x H: "); Serial.print(objBounds.x2() - objBounds.x1());Serial.print(", "); Serial.print(objBounds.y2() - objBounds.y1());
    // Serial.print(" on bank "); Serial.println(activeBank);

    graphics.readBuffer(x, y,width,height, _mouseCursorBuffer);
    if(pendingMove)
        memcpy(_mouseAreaObject->texture->colors + bufferOldOffset,_mouseCursorBuffer, width*height);
    
    
    for(int line=0;line<height; line++){
        uint8_t cursorRowPixels = _pointers[(int)_pointer][line];
        for(int row=0;row < width; row++){
            if(cursorRowPixels & (0x1 << (7-row))){
                
                _mouseCursorBuffer[(line*width) + row] = _cursorColor;
            }
            else if( 
                (line > 0 && (_pointers[(int)_pointer][line - 1]) & (0x1 << (7-row)) ) || //up
                (row > 0 && (cursorRowPixels & (0x1 << (7-(row-1)))))  || //left
                (row < width - 1 && (cursorRowPixels & (0x1 << (7-(row+1)))) )|| //right
                (line < height - 1 && (_pointers[(int)_pointer][line + 1]) & (0x1 << (7-row))) //down

            ) //if any neighbors are set, set it to oppostive of cursor color
                _mouseCursorBuffer[(line*width) + row] = _cursorColor ^ 0xFF;
                
        }
    }

    if(_mouseCursorBuffer != nullptr){
        
        auto width = objBounds.x2() - objBounds.x1() ;
        auto height = objBounds.y2() - objBounds.y1();
        
        // Serial.print("Writing mouse texture to color offset: "); Serial.print(bufferOldOffset); Serial.print(" on bank "); Serial.println(activeBank);
        memcpy(_mouseAreaObject->texture->colors + bufferCursorOffset,_mouseCursorBuffer, width* height);
        
        

        // Serial.print(" Verticies: "); Serial.print(x1);Serial.print(", "); Serial.print(y1);
        // Serial.print(" - W x H: "); Serial.print(x2 - x1);Serial.print(", "); Serial.println(y2 - y1);
        
            
        // Serial.print("Buffer old offset: "); Serial.println(bufferOldOffset);
        // Serial.print("Buffer mouse offset: "); Serial.println(bufferCursorOffset);

        //store to old
        #ifdef DEBUG_MOUSE
        Serial.print("Saving mouse texture to color offset: "); Serial.print(bufferOldOffset); Serial.print(" on bank "); Serial.println(activeBank);
        #endif
        //if(pendingMove) //grab the current view if pending move, otherwise don't store (with cursor already present)
        graphics.readBuffer(objBounds.x1(),objBounds.y1(),width,height,_mouseAreaObject->texture->colors + bufferOldOffset);
        //draw new
        #ifdef DEBUG_MOUSE
        Serial.print("Drawing mouse texture to color offset: "); Serial.print(bufferCursorOffset); Serial.print(" on bank "); Serial.println(activeBank);
        #endif
        graphics.drawBuffer(objBounds.x1(),objBounds.y1(),width,height,_mouseAreaObject->texture->colors + bufferCursorOffset);

        // Serial.print("Result old buffer data:");
        // for(int idx = 0; idx < width * height; idx++){
        //     if(idx % width == 0)
        //         Serial.println();

        //     Serial.print("0x"); Serial.print(_mouseAreaObject->texture->colors[idx + bufferOldOffset]);Serial.print(", ");
        // }
        // Serial.println();
        // Serial.print("Result new buffer data:");
        // for(int idx = 0; idx < width * height; idx++){
        //     if(idx % width == 0)
        //         Serial.println();

        //     Serial.print("0x"); Serial.print(_mouseAreaObject->texture->colors[idx + bufferCursorOffset]);Serial.print(", ");
        // }
        
        
        if(activeBank)
            _mouseAreaObject->drawnOnMem2 = true;
        else
            _mouseAreaObject->drawnOnMem1 = true;
       

    //     // Serial.print("Should draw mouse object at ");
    //     // Serial.print(_mouseAreaObject->shape->vertices[0].x);
    //     // Serial.print(", ");
    //     // Serial.print(_mouseAreaObject->shape->vertices[0].y);

    //     // Serial.print(" with width");
    //     // Serial.print(_mouseAreaObject->shape->vertices[1].x - _mouseAreaObject->shape->vertices[0].x);
    //     // Serial.print(" and height ");
    //     // Serial.print(_mouseAreaObject->shape->vertices[1].y - _mouseAreaObject->shape->vertices[0].y);
    //     // Serial.println();        
    }
}

void VGAMouse::clearCursor()
{
    //determine current bank
    // restore to proper position of that bank

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

        if(_lastData.wheel != 0){
            if(onWheel != nullptr)
                onWheel(MouseWheelArgs(_mouseLocation, _lastData.wheel));
        }
        
        pendingEvent = buttonClicked > 0; //any lower 3 bits set
        pendingMove = _lastData.position.x != 0 || _lastData.position.y != 0;
    }

    //usb 
    auto activeBank = gpu.activeBank() ;
    auto systemPending = activeBank ?_pendingMoveBank2 : _pendingMoveBank1;
    if(_initializedUsb){
        pendingEvent = _pendingEvent;
        pendingMove = systemPending;
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
        auto settings = gpu.GetSettings();
        if(!systemPending){ // update previous location is not already pending a move
            if(activeBank){
                _previousLocationBank2 = _mouseLocation;
            }
            else{
                _previousLocationBank1 = _mouseLocation;
            }
        }
        //update position
        if(_lastData.position.x > 127 ){
            _mouseLocation.x -= (256 - _lastData.position.x );
            if(_mouseLocation.x < 0) 
                _mouseLocation.x = 0;
        }else{
            _mouseLocation.x += _lastData.position.x;
            if(_mouseLocation.x > settings.screenWidth) 
                _mouseLocation.x = settings.screenWidth - 2;
        }
        if(_lastData.position.y > 127){
            _mouseLocation.y += (256 - _lastData.position.y );
            if(_mouseLocation.y > settings.screenHeight) 
                _mouseLocation.y = settings.screenHeight - 2;        
            
        }else{
            _mouseLocation.y -= _lastData.position.y;
            if(_mouseLocation.y < 0) 
                _mouseLocation.y = 0;
        }
        if(onMouseMove != nullptr)
            onMouseMove(_lastData.position.x, _lastData.position.y);
    }
    
    _pendingMoveBank1 = pendingMove;
    _pendingMoveBank2 = pendingMove;
    _pendingEvent = pendingEvent;
}

void VGAMouse::update()
{
    if(_pointer == pointerNone) return;
    if(!_initializedPs2 && !_initializedUsb) return;
    bool needRedraw = _pendingEvent || _pendingMoveBank2 || _pendingMoveBank1 || _pendingRequestRedraw;
    auto activeBank = gpu.activeBank();
    auto pendingMove = activeBank ? _pendingMoveBank2 : _pendingMoveBank1;
    //Serial.print("Pending Move: "); Serial.print(pendingMove); Serial.print(" Bank 1: "); Serial.print(_pendingMoveBank1); Serial.print(" Bank 2: "); Serial.println(_pendingMoveBank2);
    if(_pendingEvent || pendingMove || _pendingRequestRedraw){
        //Serial.print("Gpu has "); Serial.print(gpu.Get2DObjects()->size()); Serial.print(" objects on bank "); Serial.println(gpu.activeBank());
        //write out the old
        //auto _mouseAreaObject = activeBank ? _mouseAreaObjectBank2 : _mouseAreaObjectBank1;
        // Serial.print("Pending Event: "); Serial.println(_pendingEvent);
        // Serial.print("Pending Move: "); Serial.print(pendingMove); Serial.print(" Bank 1: "); Serial.print(_pendingMoveBank1); Serial.print(" Bank 2: "); Serial.println(_pendingMoveBank2);
        // Serial.print("Requested Redraw: "); Serial.println(_pendingRequestRedraw);
        
        auto objBounds = _mouseAreaObject->getBounds();
        int bufferOffset = activeBank * objBounds.height() * objBounds.width() * 2;
        auto preLocation = activeBank ? _previousLocationBank2 : _previousLocationBank1;
        if(activeBank ? _mouseAreaObject->drawnOnMem1 : _mouseAreaObject->drawnOnMem2){
            //write back if bank data should be there
            #ifdef DEBUG_MOUSE
            Serial.print("Restoring surfrace from Mouse buffer on bank ");Serial.print(activeBank); 
            Serial.print(" at "); Serial.print(_mouseAreaObject->shape->vertices[0].x);
            Serial.print(", "); Serial.println(_mouseAreaObject->shape->vertices[0].y);
            #endif
            graphics.drawBuffer(
                preLocation.x, 
                preLocation.y, 
                objBounds.width(),
                objBounds.height(), 
                _mouseAreaObject->texture->colors + bufferOffset
            );
        }
        
        if(activeBank){
            _mouseAreaObject->drawnOnMem2 = false;
        }
        else{
            _mouseAreaObject->drawnOnMem1 = false;
        }
        
        _mouseAreaObject->shape->move_to(_mouseLocation.x, _mouseLocation.y);
      
        if(activeBank)
            _previousLocationBank2 = _mouseLocation;
        else
            _previousLocationBank1 = _mouseLocation; //if we moved, update location
       
       
        //draw cursor
        drawCursor(_mouseLocation.x, _mouseLocation.y, CURSOR_SIZE, CURSOR_SIZE);
        if(activeBank){
            _pendingMoveBank2 = false;
        }
        else{
            _pendingMoveBank1 = false;
        }
        _pendingEvent = false;        
        _pendingRequestRedraw = false;
        
    }
    if(needRedraw){
        gpu.RequestRender();
    }
    
}

bool VGAMouse::tryInitializeMouse(uint8_t clk, uint8_t data)
{
    _mouse = new PS2Mouse(clk, data);
    
    if(_mouse->begin()) {
        _mouseReadTimer = new DueTimer(Timer.getAvailable());
        _mouseReadTimer->attachInterrupt(vgaMouseInputEventHandler);
        _mouseReadTimer->start(_readInterval * 1000); //check every 1ms
        //_initializedPs2 = true;
        return true;
    } else{
        return false;
    }
}
