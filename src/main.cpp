#include "Arduino.h"
#include "DueHardwareVGA.h"

bool _positionUpdated = false;
bool _mouseClicked = false;

void updatePosition(){
    _positionUpdated = true;   
}

void mouseClick(uint8_t button){
    _mouseClicked = true;
    auto pointer = mouse.getPointer();
    pointer = (MousePointer)((int)pointer + 1);
    if(pointer > 3)
        pointer = (MousePointer)0;
    mouse.setPointer(pointer);
}

void setup(){
    Serial.begin(115200);
    Serial.println("Started Due Hardware VGA Mouse Test");
    graphics.begin();
    graphics.clear();

    //background to see mouse in action
    for(int vert = 0; vert < graphics.settings.screenHeight / 10; vert++){
        graphics.fillRectangle(0,vert * 10, graphics.settings.screenWidth, graphics.settings.screenHeight / 10, 256/vert);
    }
    
    mouse.begin();    
    mouse.onMouseMove = updatePosition;
    mouse.onClick = mouseClick;
    Serial.println("Initialized");
}


void loop(){    
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

}

