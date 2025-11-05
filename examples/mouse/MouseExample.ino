#include "Arduino.h"
#include "DueHardwareVGA.h"

bool _positionUpdated = false;
bool _mouseClicked = false;
int _buttonClicked = 0;
Graphics2D _graphics;

void updatePosition(int16_t moveX, int16_t moveY){
    _positionUpdated = true;   
}

void mouseClick(MouseClickArgs args){
    _mouseClicked = true;
    _buttonClicked = args.button;
}

void setup(){
    Serial.begin(115200);
    if(Serial.availableForWrite())
        Serial.println("Started Due Hardware VGA Mouse Test");
    gpu.begin(1,1);
    
    //background to see mouse in action
    auto width = 25;
    auto height = 25;
    for(int vert = 0; vert < graphics.settings.screenHeight / height; vert++){
        for(int hor = 0; hor < graphics.settings.screenWidth / width; hor++){
            auto obj = GraphicsObject2D(new Rectangle2D(hor * width,vert * height, ((hor + 1) * width) , ((vert + 1) * height),Fill),((vert * 4) +(hor*3))% 255 );
            _graphics.shapeList->push_back(std::move(obj));        
        }
    }
    gpu.Set2DObjects(_graphics.shapeList);
    mouse.begin();    
    mouse.onMouseMove = updatePosition;
    mouse.onClick = mouseClick;
    if(Serial.availableForWrite())
    Serial.println("Initialized");
    gpu.Render();
}

unsigned long lastRendered = millis();
void loop(){    
    if(_positionUpdated){
        _positionUpdated = false;
        Serial.print("Mouse new position: (");
        Serial.print(mouse.location().x); Serial.print(" , ");
        Serial.print(mouse.location().y); Serial.println(")");
        
    }  
    if(_mouseClicked){
        _mouseClicked = false;
        Serial.println("Mouse clicked");
        if(_buttonClicked == 0){
            auto pointer = mouse.getPointer();
            pointer = (MousePointer)((int)pointer + 1);
            if(pointer > 3)
                pointer = (MousePointer)0;
            mouse.setPointer(pointer);
        }
        
    } 
    mouse.update();
    gpu.Render();
    lastRendered = millis();
    
}

