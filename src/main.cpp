#include "Arduino.h"
#include "DueHardwareVGA.h"

bool _positionUpdated = false;
bool _mouseClicked = false;
extern GPU gpu;

void updatePosition(int16_t moveX, int16_t moveY){
    _positionUpdated = true;   
}

void mouseClick(MouseClickArgs args){
    _mouseClicked = true;
    if(args.button == 0){
        auto pointer = mouse.getPointer();
        pointer = (MousePointer)((int)pointer + 1);
        if(pointer > 3)
            pointer = (MousePointer)0;
        mouse.setPointer(pointer);
    }
}

void setup(){
    Serial.begin(115200);
    Serial.println("Started Due Hardware VGA Mouse Test");
    gpu = GPU(RenderMode::Buffered);
    //graphics.begin();
    //graphics.clear();
    auto settings = gpu.GetSettings();

    //background to see mouse in action
    for(int vert = 0; vert < settings.screenHeight / 10; vert++){
        gpu.Add2DObject(GraphicsObject2D(
            Rectangle2D(0, vert * 10, settings.screenWidth, 10),
            Texture2D(1,1,new uint8_t[1]{(uint8_t)(256/vert)} )
        ));
        //graphics.fillRectangle(0,vert * 10, settings.screenWidth, settings.screenHeight / 10, 256/vert);
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

