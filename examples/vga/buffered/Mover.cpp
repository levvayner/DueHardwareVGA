#include "Arduino.h"
#include "DueHardwareVGA.h"


auto boxWidth = 20;
auto boxHeight = 20;
auto speed = 5;
auto boxColor = Color::BRICK;
Graphics2DObject * _myBox;

void consoleProcessKey(uint8_t data){
    #ifdef DEBUG
    Serial.print("Received key 0x"); Serial.println(data, HEX);
    #endif
    switch (data)
    {
        //TODO: add bounds checking
    case 0x41: //up arrow
    case PS2_KEY_UP_ARROW:                
        _myBox->move_by(0,-1 * speed); 
        //gpu.Invalidate();       
        break;
    case 0x42: //down arrow
    case PS2_KEY_DN_ARROW:        
        _myBox->move_by(0,speed);
        //gpu.Invalidate();
        break;
    case 0x43: //right arrow     
    case PS2_KEY_R_ARROW:        
        _myBox->move_by(speed,0);
        //gpu.Invalidate();
        break;
    case 0x44: //left arrow
    case PS2_KEY_L_ARROW:
        _myBox->move_by(-1 * speed,0);
        //gpu.Invalidate();
        break;    
    default:
        break;
    }
}

void setup(){
    Serial.begin(115200);
    Serial.println("Started Due Hardware VGA");
    Serial.println("Mover - buffered example");
    gpu.begin();

    keyboard.begin(Serial,10);        
    keyboard.onKeyDown = consoleProcessKey;
    Point2D boxTL = Point2D(
        graphics.settings.screenWidth / 2 - boxWidth / 2,
        graphics.settings.screenHeight / 2 - boxHeight / 2
    );

    _myBox = new Graphics2DObject(
        //shape
        new Rectangle2D(boxTL, boxWidth, boxHeight,Fill), 
        //color
        210                                
    );
    _myBox = gpu.Add2DObject(*_myBox);
    gpu.ShowFPS();
    
}
void loop(){    
    
    gpu.Render();   
}
