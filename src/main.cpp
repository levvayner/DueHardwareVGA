#include "Arduino.h"
#include "DueHardwareVGA.h"


bool _positionUpdated = false;
bool _mouseClicked = false;
bool _mouseDragged = false;
int16_t dragX = 0, dragY = 0;

void drawBackground(){
    //background to see mouse in action
    for(int vert = 0; vert < graphics.settings.screenHeight / 10; vert++){
        graphics.fillRectangle(0,vert * 10, graphics.settings.screenWidth, graphics.settings.screenHeight / 10, 256/vert);
    }
}
void mouseMove(int16_t moveX, int16_t moveY){
    _positionUpdated = true;   
}

void mouseClick(MouseClickArgs args){
    _mouseClicked = true;
    Serial.print("Clicked button: "); Serial.println(args.button);
    if(args.button == MouseButton::LEFT_BUTTON){
        auto pointer = mouse.getPointer();
        pointer = (MousePointer)((int)pointer + 1);
        if(pointer > 3)
            pointer = (MousePointer)0;
        mouse.setPointer(pointer);
    }
    else if(args.button == MouseButton::RIGHT_BUTTON){
        drawBackground();
    }
}


void mouseDrag(int16_t x, int16_t y){
    _mouseDragged = true;
    dragX = x;
    dragY = y;
}

void setup(){
    Serial.begin(115200);
    Serial.println("Started Due Hardware VGA Mouse Test");
    graphics.begin();
    graphics.clear();

    drawBackground();
    
    mouse.begin();    
    mouse.onMouseMove = mouseMove;
    mouse.onClick = mouseClick;
    mouse.onMouseDrag = mouseDrag;
    Serial.println("Initialized");
}


void loop(){    
    usb.Task();
    if(_positionUpdated){
        _positionUpdated = false;
        Serial.print("Mouse new position: (");
        Serial.print(mouse.location().x); Serial.print(" , ");
        Serial.print(mouse.location().y); Serial.println(")");
        mouse.update();
    }  
    if(_mouseClicked){
        _mouseClicked = false;
        Serial.println("Mouse clicked");
        mouse.update();
    } 
    if(_mouseDragged){
        _mouseDragged = false;
        graphics.drawRectangle(mouse.location(), Point(dragX, dragY),255);
        mouse.update();
    }

}

