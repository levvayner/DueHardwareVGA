#include "Arduino.h"
#include "hw/video/VRAM.h"

unsigned long updateFrequency = 1000;
unsigned long lastUpdated = 0;
unsigned long checkingTime = 0;

void setup(){
    Serial.begin(115200);
    Serial.println("Started Due Hardware VGA");
    graphics.begin();
}


void loop(){
    //graphics.clear();
    for(int x=0;x<320;x++){
    int my=240;
    for(float Z=-15.02;Z<15.0;Z+=0.01){
        float X=(x-159.98)/16.0;
        float R=sqrt(X*X+Z*Z);
        float Y=150.0*(1.0+sin(R)/R); // sinc(x) == sin(x)/x
        int y=320.0-(Z*4.0)-Y;
        if(y<my && y>=0){
            uint16_t c=Y;
            while(c>255)c-=255;
            graphics.drawPixel(x,y,c); // built in low-level function
            my=y;
        }
        }
    }
    graphics.setReady();
    for(;;)for(int y=0;y<240;y++)for(int x=0;x<320;x++){
        auto color = graphics.readPixel(x,y);
        if(color > 0){ // direct low-level access
            graphics.drawPixel(x,y,++color);
            
            if(!color)
                graphics.drawPixel(x,y,++color);
        } 
    }
    

}
