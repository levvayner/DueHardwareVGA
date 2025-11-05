#include "Arduino.h"
#include "DueHardwareVGA.h"

bool _positionUpdated = false;
bool _mouseClicked = false;
Graphics2D _graphics;

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
    gpu.begin();

    //_graphics.shapeList = new ShapeList<GraphicsObject2D>();

    
    //background to see mouse in action
    for(int vert = 0; vert < graphics.settings.screenHeight / 10; vert++){
        auto obj = GraphicsObject2D(new Rectangle2D(0,vert * 10, graphics.settings.screenWidth, (vert * 10) + (graphics.settings.screenHeight / 10),Fill),vert * 4);
        _graphics.shapeList->push_back(std::move(obj));        
    }
    gpu.Set2DObjects(_graphics.shapeList);
    mouse.begin();    
    mouse.onMouseMove = updatePosition;
    mouse.onClick = mouseClick;
    Serial.println("Initialized");
    //gpu.Render();
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
        //mouse.update();
    } 
    mouse.update();
    
    //graphics.fillRectangle(mouse.location().x, mouse.location().y, 8,8,Color::WHITE);

    gpu.Render();
    lastRendered = millis();
    
}

