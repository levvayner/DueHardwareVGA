#include "Arduino.h"
#include "DueHardwareVGA.h"
PS2Mouse mouse(PS2_CLK2,PS2_DATA2);
Point mouseLocation;
int zoom = 5;

void setup(){
    Serial.begin(115200);
    Serial.println("Started Due Hardware VGA Mouse Test");
    graphics.begin();
    graphics.clear();

    mouse.begin();    
    Serial.println("Initialized");
}


void loop(){
    MouseData data = mouse.readData();
    if(data.status & 0x1){
        //left mouse button        
        graphics.fillRectangle(mouseLocation.x + 1, mouseLocation.y + 1, zoom -2, zoom -2, 250);
        delay(100);
        graphics.fillRectangle(mouseLocation.x + 1, mouseLocation.y + 1, zoom -2, zoom -2, 0);
    }

    if(data.wheel != 0){
        graphics.drawRectangle(mouseLocation.x,mouseLocation.y, zoom, zoom, 0, btVertical);
        if(data.wheel > 127 )
            zoom += 256 - data.wheel;
        else
            zoom -= data.wheel;
        graphics.drawRectangle(mouseLocation.x,mouseLocation.y, zoom, zoom, 127, btVertical);
    }
    if(data.position.x != 0 || data.position.y != 0){
        //char buf[128];
        //erase
        graphics.drawRectangle(mouseLocation.x,mouseLocation.y, zoom, zoom,0, btVertical);
        //update
        if(data.position.x > 127 ){
            mouseLocation.x -= (256 - data.position.x );
            if(mouseLocation.x < 0) 
                mouseLocation.x = 0;
        }else{
            mouseLocation.x += data.position.x;
            if(mouseLocation.x > graphics.settings.screenWidth) 
                mouseLocation.x = graphics.settings.screenWidth - 2;
        }
        if(data.position.y > 127){
            mouseLocation.y += (256 - data.position.y );
            if(mouseLocation.y > graphics.settings.screenHeight) 
                mouseLocation.y = graphics.settings.screenHeight - 2;        
            
        }else{
            mouseLocation.y -= data.position.y;
            if(mouseLocation.y < 0) 
                mouseLocation.y = 0;
        }

        //draw new
        graphics.drawRectangle(mouseLocation.x,mouseLocation.y, zoom, zoom,127, btVertical);
        // sprintf(buf, "New location (%d,%d)",mouseLocation.x, mouseLocation.y   );
        // Serial.println(buf);
    }

}

